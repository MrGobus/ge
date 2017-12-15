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

typedef void (*GEerrorCallback) (int, const char*);

void geError(int code, const char* message);
int geGetErrorCode();
const char* geGetErrorMessage();
void geErrorCallback(GEerrorCallback callback);
GEerrorCallback geGetErrorCallback();

int geInit(int width, int height, const char* title);
void geTerminate();

GLFWwindow* geGetGLFWwindow();

#define GE_NONE 0
#define GE_READ_ONLY GL_READ_ONLY
#define GE_WRITE_ONLY GL_WRITE_ONLY
#define GE_READ_WRITE GL_READ_WRITE

typedef struct GE_Rect {
	int x;
	int y;
	int width;
	int height;
} GE_Rect;

typedef struct GE_Surface {
	GLuint texture;
	int width;
	int height;
	int access;
	void* ptr;
	int usageCounter;
} GE_Surface;

GE_Surface* geCreateSurface(int width, int height);
GE_Surface* geLoadImage(const char* fileName);
void geDeleteSurface(GE_Surface* surface);
void geBlitSurface(GE_Surface* surface, const GE_Rect* dstRect, const GE_Rect* srcRect);
void* geMapSurface(GE_Surface* surface, int access);
void geUnmapSurface(GE_Surface* surface);
void geUpdateSurface(GE_Surface* surface);

int geGetSurfaceWidth(const GE_Surface* surface);
int geGetSurfaceHeight(const GE_Surface* surface);
int geGetSurfaceAccess(const GE_Surface* surface);
void* geGetSurfacePtr(const GE_Surface* surface);

typedef struct GE_Glyph {
	struct GE_Glyph* next;
	wchar_t character;
	int size;
	GE_Rect rect;
	GLuint texture;
	int advance;
} GE_Glyph;

typedef struct GE_Font {
	FT_Face face;
	int size;
	GE_Glyph* cache;	
} GE_Font;

GE_Font* geLoadFont(const char* fileName, int size);
void geDeleteFont(GE_Font* font);
void geFontSize(GE_Font* font, int size);
int geGetFontSize(GE_Font* font);
void geClearFontCache(GE_Font* font);
GE_Glyph* geGetGlyph(GE_Font* font, wchar_t character);
void geDrawGlyph(GE_Glyph* glyph, int x, int y);
void geDrawUnicodeCharacter(GE_Font* font, int x, int y, wchar_t character);
void geDrawUnicodeString(GE_Font* font, int x, int y, const wchar_t* string);
void geDrawUtf8String(GE_Font* font, int x, int y, const char* string);

int geGetScreenWidth();
int geGetScreenHeight();

void geOpacity(float opacity);
float geGetOpacity();

void geColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void geGetColor(float* color);

void geRect(const GE_Rect* rect);
void geFillRect(const GE_Rect* rect);
void geLine(int x1, int y1, int x2, int y2);

void geClear();

void geBegin(GE_Surface* surface);
void geEnd();

#endif