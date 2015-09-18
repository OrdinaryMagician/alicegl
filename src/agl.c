/*
	agl.c : The core of the AliceGL library.
	(C)2015 Marisa Kirisame, UnSX Team.
	Released under the GNU Lesser General Public License (version 3).
*/

#include "agl.h"

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

void aglInit( aglContext* ctx )
{
	(void)ctx;
}

void aglQuit( aglContext* ctx )
{
	(void)ctx;
}

void aglStartQueue( aglContext* ctx, unsigned flags )
{
	(void)ctx, (void)flags;
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
	(void)ctx, (void)arr;
}

aglSampler* aglMakeSampler( aglContext* ctx, unsigned nbytes, void* data )
{
	(void)ctx, (void)nbytes, (void)data;
	return 0;
}

void aglDeleteSampler( aglContext* ctx, aglSampler* smp )
{
	(void)ctx, (void)smp;
}

aglBuffer* aglMakeBuffer( aglContext* ctx, unsigned w, unsigned h,
	unsigned flags )
{
	(void)ctx, (void)w, (void)h, (void)flags;
	return 0;
}

void aglDeleteBuffer( aglContext* ctx, aglBuffer* buf )
{
	(void)ctx, (void)buf;
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
