/*
	agl.c : The core of the AliceGL library.
	(C)2015 Marisa Kirisame, UnSX Team.
	Released under the GNU Lesser General Public License (version 3).
*/

#include "agl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* messages */

#define M_ALLOCFAIL "Null pointer returned from malloc (out of memory?)"
#define M_NURUPO    "Oh gn0! I am teh suck!"
#define M_NOOBJ     "There was an object here, but it's gone now"
#define M_ZEROQ     "Attempted to start queue with zero size"
#define M_QUEUEFUL  "No space left in queue to push data"
#define M_DRAWEMPT  "Call to aglDrawQueue with no data"
#define M_PREMAT    "Premature end to queue (bad flags?)"
#define M_FLAGPVT   "Cannot push data with current flags"
#define M_WAT       "AGL will now attempt to fit a square peg in a round hole"
#define M_NODATA    "No data has been passed to API call"
#define M_NULLREAD  "Element has no data to read"
#define M_NOVERT    "No vertices to see here"
#define M_NOBUF     "No target buffer has been set"
#define M_UNIMP     "Function not implemented"

/* warnings and errors */
#define warn(x) fn_warn(x,__FILE__,__func__,__LINE__)
#define error(x) fn_error(x,__FILE__,__func__,__LINE__)
static void fn_warn( const char *m, const char *f, const char *fn, int l )
{
	fprintf(stderr,"libagl warning in %s:%d:(%s): %s\n",f,l,fn,m);
}
static int fn_error( const char *m, const char *f, const char *fn, int l )
{
	fprintf(stderr,"libagl error in %s:%d:(%s): %s\n",f,l,fn,m);
	return 1;
}

/* library load functions, currently unused */
static void agl_loaded( void ) __attribute__((constructor));
static void agl_unloaded( void ) __attribute__((destructor));

static void agl_loaded( void )
{
}

static void agl_unloaded( void )
{
}

/* BEGIN INTERNAL RASTERIZATION CODE */

typedef struct
{
	float position[3];
	float normal[3];
	float coord[2];
	float color[4];
} vert_t;

static int boundstest( vert_t* v )
{
	if ( v->position[0] < 0.0 || v->position[0] >= 1.0 ) return 0;
	if ( v->position[1] < 0.0 || v->position[1] >= 1.0 ) return 0;
	if ( v->position[2] < 0.0 || v->position[2] >= 1.0 ) return 0;
	return 1;
}

static int culltest( aglContext* ctx, vert_t* t )
{
	if ( ctx->cullface == CL_BOTH ) return 0;
	float avgznormal = (t[0].normal[2]+t[1].normal[2]+t[2].normal[2])/3.0;
	if ( (ctx->cullface == CL_BACK) && avgznormal >= 0 ) return 0;
	if ( (ctx->cullface == CL_FRONT) && avgznormal <= 0 ) return 0;
	return 1;
}

static int stenciltest( aglContext* ctx, vert_t* v )
{
	(void)ctx, (void)v;
	return 1;
}

static int depthtest( aglContext* ctx, vert_t* v )
{
	(void)ctx, (void)v;
	return 1;
}

static void tri_draw( aglContext* ctx, vert_t* inv )
{
	vert_t outv[3];
	/* vertex shading goes here */
	if ( ctx->vertexprog )
	{
		memcpy(&ctx->vdata,&inv[0],sizeof(vert_t));
		ctx->vertexprog(ctx);
		memcpy(&outv[0],&ctx->vdata,sizeof(vert_t));
		memcpy(&ctx->vdata,&inv[1],sizeof(vert_t));
		ctx->vertexprog(ctx);
		memcpy(&outv[1],&ctx->vdata,sizeof(vert_t));
		memcpy(&ctx->vdata,&inv[2],sizeof(vert_t));
		ctx->vertexprog(ctx);
		memcpy(&outv[2],&ctx->vdata,sizeof(vert_t));
	}
	else
	{
		memcpy(&outv[0],&inv[0],sizeof(vert_t));
		memcpy(&outv[1],&inv[1],sizeof(vert_t));
		memcpy(&outv[2],&inv[2],sizeof(vert_t));
	}
	if ( ctx->rendermode == RENDER_POINTS )
	{
		aglPixel px;
		unsigned ppx, ppy;
		if ( !culltest(ctx,&outv[0]) ) return;
		if ( boundstest(&outv[0]) && stenciltest(ctx,&outv[0])
			&& depthtest(ctx,&outv[0]) )
		{
			px.r = outv[0].color[0];
			px.g = outv[0].color[1];
			px.b = outv[0].color[2];
			px.a = outv[0].color[3];
			ppx = floor(outv[0].position[0]*ctx->target->width);
			ppy = floor(outv[0].position[1]*ctx->target->height);
			aglPutColor(ctx->target,ppx,ppy,&px);
		}
		if ( boundstest(&outv[1]) && stenciltest(ctx,&outv[1])
			&& depthtest(ctx,&outv[1]) )
		{
			px.r = outv[1].color[0];
			px.g = outv[1].color[1];
			px.b = outv[1].color[2];
			px.a = outv[1].color[3];
			ppx = floor(outv[1].position[0]*ctx->target->width);
			ppy = floor(outv[1].position[1]*ctx->target->height);
			aglPutColor(ctx->target,ppx,ppy,&px);
		}
		if ( boundstest(&outv[1]) && stenciltest(ctx,&outv[1])
			&& depthtest(ctx,&outv[1]) )
		{
			px.r = outv[2].color[0];
			px.g = outv[2].color[1];
			px.b = outv[2].color[2];
			px.a = outv[2].color[3];
			ppx = floor(outv[2].position[0]*ctx->target->width);
			ppy = floor(outv[2].position[1]*ctx->target->height);
			aglPutColor(ctx->target,ppx,ppy,&px);
		}
	}
	else if ( ctx->rendermode == RENDER_EDGES )
	{
	}
	else if ( ctx->rendermode == RENDER_FACES )
	{
	}
}

/* END INTERNAL RASTERIZATION CODE */

/* creates a new, zero-filled context object */
aglContext* aglInit( void )
{
	aglContext *ctx = malloc(sizeof(aglContext));
	if ( !ctx )
	{
		error(M_ALLOCFAIL);
		return 0;
	};
	memset(ctx,0,sizeof(aglContext));
	return ctx;
}

/* frees the context object along with all the data associated with it */
int aglQuit( aglContext* ctx )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( ctx->queue ) free(ctx->queue);
	if ( ctx->samplers )
	{
		aglSampler *s = ctx->samplers, *n;
		do
		{
			n = s->next;
			aglDeleteSampler(ctx,s);
			s = n;
		} while ( n );
	}
	if ( ctx->arrays )
	{
		aglArray *s = ctx->arrays, *n;
		do
		{
			n = s->next;
			aglDeleteArray(ctx,s);
			s = n;
		} while ( n );
	}
	if ( ctx->buffers )
	{
		aglBuffer *s = ctx->buffers, *n;
		do
		{
			n = s->next;
			aglDeleteBuffer(ctx,s);
			s = n;
		} while ( n );
	}
	free(ctx);
	return 0;
}

/* begins a new queue */
int aglStartQueue( aglContext* ctx, unsigned siz, unsigned flags )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !ctx->queue )
	{
		if ( !siz ) return error(M_ZEROQ);
		ctx->queue = malloc(sizeof(float)*siz);
		if ( !ctx->queue ) return error(M_ALLOCFAIL);
	}
	else if ( ctx->queuesize != siz )
	{
		ctx->queue = realloc(ctx->queue,sizeof(float)*siz);
		if ( !ctx->queue ) return error(M_ALLOCFAIL);
	}
	ctx->queuesize = siz;
	ctx->queuepos = 0;
	ctx->queueflags = flags;
	return 0;
}

int aglPushVertex( aglContext* ctx, float x, float y, float z )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !ctx->queue ) return error(M_NOOBJ);
	if ( !ctx->queueflags&ARR_VERTICES ) return error(M_FLAGPVT);
	if ( ctx->queuepos+3 < ctx->queuesize ) return error(M_QUEUEFUL);
	ctx->queue[ctx->queuepos++] = x;
	ctx->queue[ctx->queuepos++] = y;
	ctx->queue[ctx->queuepos++] = z;
	return 0;
}

int aglPushNormal( aglContext* ctx, float x, float y, float z )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !ctx->queue ) return error(M_NOOBJ);
	if ( !ctx->queueflags&ARR_NORMALS ) return error(M_FLAGPVT);
	if ( ctx->queuepos+3 < ctx->queuesize ) return error(M_QUEUEFUL);
	ctx->queue[ctx->queuepos++] = x;
	ctx->queue[ctx->queuepos++] = y;
	ctx->queue[ctx->queuepos++] = z;
	return 0;
}

int aglPushCoord( aglContext* ctx, float u, float v )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !ctx->queue ) return error(M_NOOBJ);
	if ( !ctx->queueflags&ARR_COORDS ) return error(M_FLAGPVT);
	if ( ctx->queuepos+2 < ctx->queuesize ) return error(M_QUEUEFUL);
	ctx->queue[ctx->queuepos++] = u;
	ctx->queue[ctx->queuepos++] = v;
	return 0;
}

int aglPushColor( aglContext* ctx, float r, float g, float b, float a )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !ctx->queue ) return error(M_NOOBJ);
	if ( !ctx->queueflags&ARR_COLORS ) return error(M_FLAGPVT);
	if ( ctx->queuepos+4 < ctx->queuesize ) return error(M_QUEUEFUL);
	ctx->queue[ctx->queuepos++] = r;
	ctx->queue[ctx->queuepos++] = g;
	ctx->queue[ctx->queuepos++] = b;
	ctx->queue[ctx->queuepos++] = a;
	return 0;
}

int aglDrawQueue( aglContext* ctx )
{
	(void)ctx;
	return error(M_UNIMP);
}

aglArray* aglMakeArray( aglContext* ctx, unsigned ntris, unsigned flags,
	void* data )
{
	(void)ctx, (void)ntris, (void)flags, (void)data;
	error(M_UNIMP);
	return 0;
}

int aglDrawArray( aglContext* ctx, aglArray* arr )
{
	(void)ctx, (void)arr;
	return error(M_UNIMP);
}

int aglDeleteArray( aglContext* ctx, aglArray* arr )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !arr ) return error(M_NOOBJ);
	if ( arr == ctx->arrays ) ctx->arrays = arr->next;
	if ( arr->prev ) arr->prev->next = arr->next;
	if ( arr->next ) arr->next->prev = arr->prev;
	if ( arr->data ) free(arr->data);
	free(arr);
	return 0;
}

aglSampler* aglMakeSampler( aglContext* ctx, void* data )
{
	if ( !ctx ) { error(M_NURUPO); return 0; }
	aglSampler *smp = malloc(sizeof(aglSampler));
	if ( !smp ) { error(M_ALLOCFAIL); return 0; }
	memset(smp,0,sizeof(aglSampler));
	if ( !data ) warn(M_NODATA);
	smp->data = data;
	aglSampler *s;
	smp->next = s = ctx->samplers;
	if ( s ) s->prev = smp;
	return smp;
}

int aglDeleteSampler( aglContext* ctx, aglSampler* smp )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !smp ) return error(M_NOOBJ);
	if ( smp == ctx->samplers ) ctx->samplers = smp->next;
	if ( smp->prev ) smp->prev->next = smp->next;
	if ( smp->next ) smp->next->prev = smp->prev;
	if ( smp->data && !smp->type&SMP_BUF ) free(smp->data);
	free(smp);
	return 0;
}

aglBuffer* aglMakeBuffer( aglContext* ctx, unsigned w, unsigned h,
	unsigned flags )
{
	if ( !ctx ) { error(M_NURUPO); return 0; }
	aglBuffer *buf = malloc(sizeof(aglBuffer));
	if ( !buf ) { error(M_ALLOCFAIL); return 0; };
	memset(buf,0,sizeof(aglBuffer));
	buf->flags = flags;
	buf->width = w;
	buf->height = h;
	unsigned pxsiz = ((flags&BUF_FLOATCOL)?sizeof(float):sizeof(char))
		*((flags&BUF_HASALPHA)?4:3);
	buf->color = malloc(w*h*pxsiz);
	if ( !buf->color ) { error(M_ALLOCFAIL); return 0; };
	pxsiz = (flags&BUF_INTDEPTH)?sizeof(unsigned):sizeof(float);
	if ( flags&BUF_USEDEPTH ) buf->depth = malloc(w*h*pxsiz);
	if ( !buf->depth ) { error(M_ALLOCFAIL); return 0; };
	pxsiz = (flags&BUF_INTSTENCIL)?sizeof(unsigned):1;
	if ( flags&BUF_USESTENCIL ) buf->stencil = malloc(w*h*pxsiz);
	if ( !buf->stencil ) { error(M_ALLOCFAIL); return 0; };
	buf->coldepmasks = -1;
	buf->stencilmask = -1;
	aglBuffer *s;
	buf->next = s = ctx->buffers;
	if ( s ) s->prev = buf;
	return buf;
}

int aglDeleteBuffer( aglContext* ctx, aglBuffer* buf )
{
	if ( !ctx ) return error(M_NURUPO);
	if ( !buf ) return error(M_NOOBJ);
	if ( buf == ctx->buffers ) ctx->buffers = buf->next;
	if ( buf->prev ) buf->prev->next = buf->next;
	if ( buf->next ) buf->next->prev = buf->prev;
	if ( buf->color ) free(buf->color);
	if ( buf->depth ) free(buf->depth);
	if ( buf->stencil ) free(buf->stencil);
	free(buf);
	return 0;
}

int aglShadeBuffer( aglContext* ctx, aglBuffer* buf, int (*prog)(aglContext*) )
{
	(void)ctx, (void)buf, (void)prog;
	return error(M_UNIMP);
}

int aglClearBuffer( aglBuffer* buf, unsigned what )
{
	(void)buf, (void)what;
	return error(M_UNIMP);
}

int aglTex2D( aglSampler* smp, float u, float v, aglPixel* to )
{
	(void)smp, (void)u, (void)v, (void)to;
	return error(M_UNIMP);
}

int aglTex3D( aglSampler* smp, float u, float v, float w, aglPixel* to )
{
	(void)smp, (void)u, (void)v, (void)w, (void)to;
	return error(M_UNIMP);
}

int aglGetPixel( aglBuffer* buf, unsigned x, unsigned y, aglPixel* to )
{
	(void)buf, (void)x, (void)y, (void)to;
	return error(M_UNIMP);
}

int aglPutPixel( aglBuffer* buf, unsigned x, unsigned y, aglPixel* from )
{
	(void)buf, (void)x, (void)y, (void)from;
	return error(M_UNIMP);
}

int aglGetColor( aglBuffer* buf, unsigned x, unsigned y, aglPixel* to )
{
	(void)buf, (void)x, (void)y, (void)to;
	return error(M_UNIMP);
}

int aglPutColor( aglBuffer* buf, unsigned x, unsigned y, aglPixel* from )
{
	(void)buf, (void)x, (void)y, (void)from;
	return error(M_UNIMP);
}

int aglGetDepth( aglBuffer* buf, unsigned x, unsigned y, aglPixel* to )
{
	(void)buf, (void)x, (void)y, (void)to;
	return error(M_UNIMP);
}

int aglPutDepth( aglBuffer* buf, unsigned x, unsigned y, aglPixel* from )
{
	(void)buf, (void)x, (void)y, (void)from;
	return error(M_UNIMP);
}

int aglGetStencil( aglBuffer* buf, unsigned x, unsigned y, aglPixel* to )
{
	(void)buf, (void)x, (void)y, (void)to;
	return error(M_UNIMP);
}

int aglPutStencil( aglBuffer* buf, unsigned x, unsigned y, aglPixel* from )
{
	(void)buf, (void)x, (void)y, (void)from;
	return error(M_UNIMP);
}
