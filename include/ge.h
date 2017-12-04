#ifndef _GE_H_
#define _GE_H_

#include <iconv.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <IL/il.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define GE_NULL 0

#define GE_FALSE 0
#define GE_TRUE 1

#define GE_OK 0
#define GE_ERROR 1

typedef GLchar GEchar;

typedef GLint GEint;
typedef GLuint GEuint;

typedef GLfloat GEfloat;

typedef GLvoid GEvoid;

typedef wchar_t GEunicodeCharacter;

typedef void (*GEerrorCallback) (GEint, const GEchar*);

void geError(GEint code, const GEchar* message);
GEint geGetErrorCode();
const GEchar* geGetErrorMessage();
void geSetErrorCallback(GEerrorCallback callback);
GEerrorCallback geGetErrorCallback();

GEint geInit(GEint width, GEint height, const GEchar* title);
void geTerminate();

GLFWwindow* geGetGLFWwindow();

#define GE_NONE 0
#define GE_READ_ONLY GL_READ_ONLY
#define GE_WRITE_ONLY GL_WRITE_ONLY
#define GE_READ_WRITE GL_READ_WRITE

typedef struct GErect {
	GEint x;
	GEint y;
	GEint width;
	GEint height;
} GErect;

typedef struct GEsurface {
	GLuint texture;
	GEint width;
	GEint height;
	GEint access;
	GEvoid* ptr;
	GEint usageCounter;
} GEsurface;

GEsurface* geCreateSurface(GEint width, GEint height);
GEsurface* geLoadImage(const GEchar* fileName);
void geDeleteSurface(GEsurface* surface);
void geBlitSurface(GEsurface* surface, const GErect* dstRect, const GErect* srcRect);
void* geMapSurface(GEsurface* surface, int access);
void geUnmapSurface(GEsurface* surface);
void geUpdateSurface(GEsurface* surface);

GEint geGetSurfaceWidth(const GEsurface* surface);
GEint geGetSurfaceHeight(const GEsurface* surface);
GEint geGetSurfaceAccess(const GEsurface* surface);
GEvoid* geGetSurfacePtr(const GEsurface* surface);

typedef struct GEglyph {
	struct GEglyph* next;
	GEunicodeCharacter character;
	GEint size;
	GErect rect;
	GLuint texture;
	GEint advance;
} GEglyph;

typedef struct GEfont {
	FT_Face face;
	GEint size;
	GEglyph* cache;	
} GEfont;

GEfont* geLoadFont(const GEchar* fileName, GEint size);
void geDeleteFont(GEfont* font);
void geSetFontSize(GEfont* font, GEint size);
GEint geGetFontSize(GEfont* font);
void geClearFontCache(GEfont* font);
GEglyph* geGetGlyph(GEfont* font, GEunicodeCharacter character);
void geDrawGlyph(GEglyph* glyph, GEint x, GEint y);
void geDrawUnicodeCharacter(GEfont* font, GEint x, GEint y, GEunicodeCharacter character);
void geDrawUnicodeString(GEfont* font, GEint x, GEint y, const GEunicodeCharacter* string);
void geDrawUtf8String(GEfont* font, GEint x, GEint y, const GEchar* string);

GEint geGetScreenWidth();
GEint geGetScreenHeight();

void geSetOpacity(GEfloat opacity);
GEfloat geGetOpacity();

void geSetColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void geGetColor(GEfloat* color);

void geClear();

void geBegin(GEsurface* surface);
void geEnd();

#endif