//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef NANOVG_GL_H
#define NANOVG_GL_H

#if defined(USE_OPENGL)

#if defined(__cplusplus)
extern "C" {
#endif

// Create flags

enum NVGcreateFlags {
    // Flag indicating if geometry based anti-aliasing is used (may not be needed when using MSAA).
    NVG_ANTIALIAS 		= 1<<0,
    // Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
    // slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
    NVG_STENCIL_STROKES	= 1<<1,
    // Flag indicating that additional debug checks are done.
    NVG_DEBUG 			= 1<<2,
};

#if defined NANOVG_GL2_IMPLEMENTATION
#  define NANOVG_GL2 1
#  define NANOVG_GL_IMPLEMENTATION 1
#elif defined NANOVG_GL3_IMPLEMENTATION
#  define NANOVG_GL3 1
#  define NANOVG_GL_IMPLEMENTATION 1
#  define NANOVG_GL_USE_UNIFORMBUFFER 1
#elif defined NANOVG_GLES2_IMPLEMENTATION
#  define NANOVG_GLES2 1
#  define NANOVG_GL_IMPLEMENTATION 1
#elif defined NANOVG_GLES3_IMPLEMENTATION
#  define NANOVG_GLES3 1
#  define NANOVG_GL_IMPLEMENTATION 1
#endif

#define NANOVG_GL_USE_STATE_FILTER (1)

// Creates NanoVG contexts for different OpenGL (ES) versions.
// Flags should be combination of the create flags above.

#if defined NANOVG_GL2

NVGcontext* nvgCreateGL2(int flags);
void nvgDeleteGL2(NVGcontext* ctx);

int nvglCreateImageFromHandleGL2(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGL2(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GL3

NVGcontext* nvgCreateGL3(int flags);
void nvgDeleteGL3(NVGcontext* ctx);

int nvglCreateImageFromHandleGL3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGL3(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GLES2

NVGcontext* nvgCreateGLES2(int flags);
void nvgDeleteGLES2(NVGcontext* ctx);

int nvglCreateImageFromHandleGLES2(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGLES2(NVGcontext* ctx, int image);

#endif

#if defined NANOVG_GLES3

NVGcontext* nvgCreateGLES3(int flags);
void nvgDeleteGLES3(NVGcontext* ctx);

int nvglCreateImageFromHandleGLES3(NVGcontext* ctx, GLuint textureId, int w, int h, int flags);
GLuint nvglImageHandleGLES3(NVGcontext* ctx, int image);

#endif

// These are additional flags on top of NVGimageFlags.
enum NVGimageFlagsGL {
    NVG_IMAGE_NODELETE			= 1<<16,	// Do not delete GL texture handle.
};

#if defined(__cplusplus)
}
#endif

#endif /* NANOVG_GL_H */

#if defined(NANOVG_GL_IMPLEMENTATION)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nanovg.h"

enum GLNVGuniformLoc {
    GLNVG_LOC_VIEWSIZE,
    GLNVG_LOC_TEX,
    GLNVG_LOC_FRAG,
    GLNVG_MAX_LOCS
};

enum GLNVGshaderType {
    NSVG_SHADER_FILLGRAD,
    NSVG_SHADER_FILLIMG,
    NSVG_SHADER_SIMPLE,
    NSVG_SHADER_IMG
};

#if NANOVG_GL_USE_UNIFORMBUFFER
enum GLNVGuniformBindings {
    GLNVG_FRAG_BINDING = 0,
};
#endif

struct GLNVGshader {
    GLuint prog;
    GLuint frag;
    GLuint vert;
    GLint loc[GLNVG_MAX_LOCS];
};
typedef struct GLNVGshader GLNVGshader;

struct GLNVGtexture {
    int id;
    GLuint tex;
    int width, height;
    int type;
    int flags;
};
typedef struct GLNVGtexture GLNVGtexture;

struct GLNVGblend
{
    GLenum srcRGB;
    GLenum dstRGB;
    GLenum srcAlpha;
    GLenum dstAlpha;
};
typedef struct GLNVGblend GLNVGblend;

enum GLNVGcallType {
    GLNVG_NONE = 0,
    GLNVG_FILL,
    GLNVG_CONVEXFILL,
    GLNVG_STROKE,
    GLNVG_TRIANGLES,
};

struct GLNVGcall {
    int type;
    int image;
    int pathOffset;
    int pathCount;
    int triangleOffset;
    int triangleCount;
    int uniformOffset;
    GLNVGblend blendFunc;
};
typedef struct GLNVGcall GLNVGcall;

struct GLNVGpath {
    int fillOffset;
    int fillCount;
    int strokeOffset;
    int strokeCount;
};
typedef struct GLNVGpath GLNVGpath;

struct GLNVGfragUniforms {
    #if NANOVG_GL_USE_UNIFORMBUFFER
        float scissorMat[12]; // matrices are actually 3 vec4s
        float paintMat[12];
        struct NVGcolor innerCol;
        struct NVGcolor outerCol;
        float scissorExt[2];
        float scissorScale[2];
        float extent[2];
        float radius;
        float feather;
        float strokeMult;
        float strokeThr;
        int texType;
        int type;
    #else
        // note: after modifying layout or size of uniform array,
        // don't forget to also update the fragment shader source!
        #define NANOVG_GL_UNIFORMARRAY_SIZE 11
        union {
            struct {
                float scissorMat[12]; // matrices are actually 3 vec4s
                float paintMat[12];
                struct NVGcolor innerCol;
                struct NVGcolor outerCol;
                float scissorExt[2];
                float scissorScale[2];
                float extent[2];
                float radius;
                float feather;
                float strokeMult;
                float strokeThr;
                float texType;
                float type;
            };
            float uniformArray[NANOVG_GL_UNIFORMARRAY_SIZE][4];
        };
    #endif
};
typedef struct GLNVGfragUniforms GLNVGfragUniforms;

struct GLNVGcontext {
    GLNVGshader shader;
    GLNVGtexture* textures;
    float view[2];
    int ntextures;
    int ctextures;
    int textureId;
    GLuint vertBuf;
#if defined NANOVG_GL3
    GLuint vertArr;
#endif
#if NANOVG_GL_USE_UNIFORMBUFFER
    GLuint fragBuf;
#endif
    int fragSize;
    int flags;

    // Per frame buffers
    GLNVGcall* calls;
    int ccalls;
    int ncalls;
    GLNVGpath* paths;
    int cpaths;
    int npaths;
    struct NVGvertex* verts;
    int cverts;
    int nverts;
    unsigned char* uniforms;
    int cuniforms;
    int nuniforms;

    // cached state
    #if NANOVG_GL_USE_STATE_FILTER
    GLuint boundTexture;
    GLuint stencilMask;
    GLenum stencilFunc;
    GLint stencilFuncRef;
    GLuint stencilFuncMask;
    GLNVGblend blendFunc;
    #endif

    int dummyTex;
};
typedef struct GLNVGcontext GLNVGcontext;

#endif /* NANOVG_GL_IMPLEMENTATION */
#endif /* USE_OPENGL */
