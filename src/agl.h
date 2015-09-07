/*
	agl.h : The core header of AliceGL, with all types and stuff.
	(C)2015 Marisa Kirisame, UnSX Team.
	Released under the GNU Lesser General Public License (version 3).
*/

#define BUF_HASALPHA 0x01
#define BUF_FLOATCOL 0x02
#define BUF_USEDEPTH 0x04
#define BUF_INTDEPTH 0x08

#define ARR_VERTICES 0x01
#define ARR_NORMALS  0x02
#define ARR_COORDS   0x04
#define ARR_COLORS   0x08

#define SMP_COORD_CLAMP  0x00
#define SMP_COORD_BORDER 0x01
#define SMP_COORD_WRAP   0x02
#define SMP_COORD_MIRROR 0x03

#define SMP_FILTER_NEAREST 0x00
#define SMP_FILTER_LINEAR  0x01

#define SMP_DEPTH_RGB8  0x00
#define SMP_DEPTH_RGBA8 0x01
#define SMP_DEPTH_RGBF  0x02
#define SMP_DEPTH_RGBAF 0x03

#define SMP_UNSET  0x00
#define SMP_FLAT   0x01
#define SMP_CUBE   0x02
#define SMP_VOLUME 0x03

#define CUBE_PX 0x00
#define CUBE_NX 0x01
#define CUBE_PY 0x02
#define CUBE_NY 0x03
#define CUBE_PZ 0x04
#define CUBE_NZ 0x05

/* a sampler */
typedef struct aglsmp
{
	struct aglsmp *prev, *next;
	void *data;
	unsigned width, height, length;
	unsigned char depth;
	unsigned char coordu, coordv, coordw;
	unsigned char filter;
	unsigned char type;
	float border[4];
} aglSampler;

/* a buffer */
typedef struct aglbuf
{
	struct aglbuf *prev, *next;
	void *color, *depth;
	unsigned width, height;
	unsigned char flags;
} aglBuffer;
/* an array */
typedef struct aglarr
{
	struct aglarr *prev, *next;
	void *data;
	unsigned ntris;
	unsigned char flags;
} aglArray;

/* data that gets passed to and from transform programs */
typedef struct
{
	float pos[3];
	float normal[3];
	float texcoord[2];
	float color[4];
	float uniform[16];
} aglProgdata;

/* the context, because we don't need global state */
typedef struct aglctx
{
	aglBuffer *target;
	aglSampler *activetex[8];
	unsigned char depthtest;
	unsigned char blendsrc;
	unsigned char blenddest;
	float *queue;
	unsigned queuesize, queuepos;
	unsigned char queueflags;
	aglSampler *samplers;
	aglArray *arrags;
	aglBuffer *buffers;
	aglProgdata pdata;
	void (*vertexprog)(struct aglctx* ctx);
	void (*fragmentprog)(struct aglctx* ctx);
	void (*bufferprog)(struct aglctx* ctx);
} aglContext;

/* initialize a context (allocates struct and sets defaults */
void aglInit( aglContext* ctx );
/* destroy a context (clears resources then frees context) */
void aglQuit( aglContext* ctx );

/* start up the geometry queue (clears any existing data) */
void aglStartQueue( aglContext* ctx, unsigned flags );
/* push various elements to the queue (make sure you follow the order) */
void aglPushVertex( aglContext* ctx, float x, float y, float z );
void aglPushNormal( aglContext* ctx, float x, float y, float z );
void aglPushCoord( aglContext* ctx, float u, float v );
void aglPushColor( aglContext* ctx, float r, float g, float b );
/* rasterize all the things! */
void aglDrawQueue( aglContext* ctx );
/* note that alicegl only knows triangles. no strips, fans, or any others */

/* draw arrays are simpler and faster (for alicegl, not for you!) */
aglArray* aglMakeArray( aglContext* ctx, unsigned ntris, unsigned flags,
	void* data );
void aglDrawArray( aglContext* ctx, aglArray* arr );
void aglDeleteArray( aglContext* ctx, aglArray* arr );

/* samplers are a fancy way of calling textures */
aglSampler* aglMakeSampler( aglContext* ctx, unsigned nbytes, void* data );
void aglDeleteSampler( aglContext* ctx, aglSampler* smp );

/* buffers are render targets, opengl calls them frame buffer objects  */
aglBuffer* aglMakeBuffer( aglContext* ctx, unsigned w, unsigned h,
	unsigned flags );
void aglDeleteBuffer( aglContext* ctx, aglBuffer* buf );

/* texture sampling functions */

void aglTex2D( aglSampler* smp, float u, float v, void* to );
void aglTex3D( aglSampler* smp, float u, float v, float w, void *to );
void aglGetPixel( aglBuffer* buf, unsigned x, unsigned y, void* to );
void aglPutPixel( aglBuffer* buf, unsigned x, unsigned y, void* from );