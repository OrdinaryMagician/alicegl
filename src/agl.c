/*
	agl.c : The core of the AliceGL library.
	(C)2015 Marisa Kirisame, UnSX Team.
	Released under the GNU Lesser General Public License (version 3).
*/

#include "agl.h"
#include <stdlib.h>
#include <string.h>

/* prototypes */

/* library load functions */
static void agl_loaded( void ) __attribute__((constructor));
static void agl_unloaded( void ) __attribute__((destructor));

static void agl_loaded( void )
{
}

static void agl_unloaded( void )
{
}

/* creates a new, zero-filled context object */
aglContext* aglInit( void )
{
	aglContext *ctx = malloc(sizeof(aglContext));
	memset(ctx,0,sizeof(aglContext));
	return ctx;
}

/* frees the context object along with all the data associated with it */
void aglQuit( aglContext* ctx )
{
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
}

/* begins a new queue */
void aglStartQueue( aglContext* ctx, unsigned flags )
{
	if ( !ctx->queue )
		ctx->queue = malloc(sizeof(float)*(ctx->queuesize=256));
	ctx->queuepos = 0;
	ctx->queueflags = flags;
	// left here
}

void aglPushVertex( aglContext* ctx, float x, float y, float z )
{
	(void)ctx, (void)x, (void)y, (void)z;
}

void aglPushNormal( aglContext* ctx, float x, float y, float z )
{
	(void)ctx, (void)x, (void)y, (void)z;
}

void aglPushCoord( aglContext* ctx, float u, float v )
{
	(void)ctx, (void)u, (void)v;
}

void aglPushColor( aglContext* ctx, float r, float g, float b )
{
	(void)ctx, (void)r, (void)g, (void)b;
}

void aglDrawQueue( aglContext* ctx )
{
	(void)ctx;
}

aglArray* aglMakeArray( aglContext* ctx, unsigned ntris, unsigned flags,
	void* data )
{
	(void)ctx, (void)ntris, (void)flags, (void)data;
	return 0;
}

void aglDrawArray( aglContext* ctx, aglArray* arr )
{
	(void)ctx, (void)arr;
}

void aglDeleteArray( aglContext* ctx, aglArray* arr )
{
	if ( arr == ctx->arrays ) ctx->arrays = arr->next;
	if ( arr->prev ) arr->prev->next = arr->next;
	if ( arr->next ) arr->next->prev = arr->prev;
	if ( arr->data ) free(arr->data);
	free(arr);
}

aglSampler* aglMakeSampler( aglContext* ctx, void* data )
{
	aglSampler *smp = malloc(sizeof(aglSampler));
	memset(smp,0,sizeof(aglSampler));
	smp->data = data;
	aglSampler *s;
	smp->next = s = ctx->samplers;
	return (s->prev = smp);
}

void aglDeleteSampler( aglContext* ctx, aglSampler* smp )
{
	if ( smp == ctx->samplers ) ctx->samplers = smp->next;
	if ( smp->prev ) smp->prev->next = smp->next;
	if ( smp->next ) smp->next->prev = smp->prev;
	if ( smp->data ) free(smp->data);
	free(smp);
}

aglBuffer* aglMakeBuffer( aglContext* ctx, unsigned w, unsigned h,
	unsigned flags )
{
	aglBuffer *buf = malloc(sizeof(aglBuffer));
	memset(buf,0,sizeof(aglBuffer));
	buf->flags = flags;
	buf->width = w;
	buf->height = h;
	unsigned pxsiz = ((flags&BUF_FLOATCOL)?sizeof(float):sizeof(char))
		*((flags&BUF_HASALPHA)?4:3);
	buf->color = malloc(w*h*pxsiz);
	pxsiz = (flags&BUF_INTDEPTH)?sizeof(unsigned):sizeof(float);
	if ( flags&BUF_USEDEPTH ) buf->depth = malloc(w*h*pxsiz);
	pxsiz = (flags&BUF_INTSTENCIL)?sizeof(unsigned):1;
	if ( flags&BUF_USESTENCIL ) buf->stencil = malloc(w*h*pxsiz);
	buf->coldepmasks = -1;
	buf->stencilmask = -1;
	aglBuffer *s;
	buf->next = s = ctx->buffers;
	return (s->prev = buf);
}

void aglDeleteBuffer( aglContext* ctx, aglBuffer* buf )
{
	if ( buf == ctx->buffers ) ctx->buffers = buf->next;
	if ( buf->prev ) buf->prev->next = buf->next;
	if ( buf->next ) buf->next->prev = buf->prev;
	if ( buf->color ) free(buf->color);
	if ( buf->depth ) free(buf->depth);
	if ( buf->stencil ) free(buf->stencil);
	free(buf);
}

void aglTex2D( aglSampler* smp, float u, float v, void* to )
{
	(void)smp, (void)u, (void)v, (void)to;
}

void aglTex3D( aglSampler* smp, float u, float v, float w, void *to )
{
	(void)smp, (void)u, (void)v, (void)w, (void)to;
}

void aglGetPixel( aglBuffer* buf, unsigned x, unsigned y, void* to )
{
	(void)buf, (void)x, (void)y, (void)to;
}

void aglPutPixel( aglBuffer* buf, unsigned x, unsigned y, void* from )
{
	(void)buf, (void)x, (void)y, (void)from;
}
