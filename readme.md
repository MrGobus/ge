# 2D Graphic Engine GE

Библиотека GE содержит набор функций для построения 2D графики.

* Вывод изображении (surface, поверхности)
* Загрузка изображений из файла
* Построение (минимальное - линии, прямоугольники)
* Вывод текста
* Рендер на поверхность

Используемые библиотеки

* [GLFW3](http://www.glfw.org/) - создание окна, клавиатура, мышь и т.п. 
* [GLEW](http://glew.sourceforge.net/) - получение функционала OpenGL 3.3
* [DevIL](http://openil.sourceforge.net/) - загрузка изображений
* [FreeType](https://www.freetype.org/) - шрифты

# Сборка

Порядок сборки:

```
make
make install
```

Прилагаемый makefile создан под [MSYS2](http://www.msys2.org/) - набор утилит и консоль для компилятора MinGW64.

Для сборки в других средах и на других платформах не подойдет, так как могут быть использованы другие имена библиотек (например -lgl вместо -lopengl32 для linux) а также другие системные пути.

makefile для сборки приложения с использованием библиотеки ge

```
CC = gcc
CFLAGS = -Wall `pkg-config --cflags freetype2` -O3
LFLAGS = -lge -lglfw3 -lglew32 -lopengl32 -lIL -lfreetype -liconv
TARGET = main.exe

default: clear
	$(CC) $(CFLAGS) -c main.c
	$(CC) main.o $(LFLAGS) -o $(TARGET)
	
clear:
	rm -rf *.o
	rm -rf $(TARGET)
```

# Пример кода

```c
#include <ge.h>

/**
	Обработчик ошибки
	@param code - код ошибки
	@param message - сообщение об ошибке
*/

void errorCallback(int code, const char* message) {
	fprintf(stderr, "error: %d\n%s\n", code, message);
	geTerminate();
	exit(code);
}

int main() {
	if (geInit(640, 480, "Hello") == GE_OK) {
		GLFWwindow* window = geGetGLFWwindow();
		
		GE_Surface* surface = geLoadImage("image.png");
		
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			geBegin(NULL);
				GE_Rect dstRect = {
					10, 10,
					geGetSurfaceWidth(surface), geGetSurfaceHeight(surface)
				};
				geBlitSurface(surface, &dstRect, NULL);
			geEnd();
			glfwSwapBuffers(window);
		}
		
		geDeleteSurface(surface);
		geTerminate();		
	}
	return 0;
}
```

# Список функций

## Обработка ошибок

Тип для обработчика ошибок.

```c
typedef void (*GEerrorCallback) (int, const char*)
```

Генерация ошибки. 
Параметры код ошибки и сообщение. Вызовет обработчик ошибки если он установлен.

```c
void geError(int code, const char* message);
```

Получить код последней ошибки.

```c
int geGetErrorCode();
```

```
GE_NONE - нет ошибки
GE_ERROR - ошибка
```

Получить сообщение об ошибке.

```c
const char* geGetErrorMessage();
```

Установить обработчик ошибок.

```c
void geErrorCallback(GEerrorCallback callback);
```

Получить обработчик ошибок.

```c
GEerrorCallback geGetErrorCallback();
```

## Инициализация

Инициализация библиотеки.
Параметры - размер окна и заголовок.

```c
int geInit(int width, int height, const char* title);
```

Завершение работы библиотеки.

```c
void geTerminate();
```

Возвращает окно GLFW созданное во время инициализации

```c
GLFWwindow* geGetGLFWwindow();
```

## Типы

Описывает прямоугольную область по координатам x, y размерами width, height

```c
typedef struct GE_Rect {
	int x;
	int y;
	int width;
	int height;
} GE_Rect;
```

## Рисование

Начать процесс рисования. Если surface == NULL вывод будет в окно иначе в заданную поверхность.

```c
void geBegin(GE_Surface* surface);
```

Завершить процесс рисования

```c
void geEnd();
```

Нарисует прямоугольник

```c
void GE_Rect(const GE_Rect* rect);
```

Нарисует закрашенный прямоугольник

```c
void geFillRect(const GE_Rect* rect);
```

Нарисует линию

```c
void geLine(int x1, int y1, int x2, int y2);
```

Вернет ширину экрана - окна либо поверхности на которую производится рендер.

```c
int geGetScreenWidth();
```

Вернет высоту экрана - окна либо поверхности на которую производится рендер.

```c
int geGetScreenHeight();
```

Установить глобальную прозрачность.

```c
void geOpacity(float opacity);
```

Вернет глобальную прозрачность

```c
float geGetOpacity();
```

Установить цвет закраски (прямоугольники, линии, текст, очистка экрана)

```c
void geColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
```

Получить цвет закраски

```c
void geGetColor(float* color);
```

Очистить экран

```c
void geClear();
```

## Поверхность

Поверхность, это прямоугольная область хранящая графические данные. Вся поверхность либо ее часть может быть скопирована на экран либо на другую поверхность.

Тип поверхности.

```c
typedef struct GE_Surface {
	GLuint texture;
	int width;
	int height;
	int access;
	void* ptr;
	int usageCounter;
} GE_Surface;
```

Создать поверхность заданного размера.

```c
GE_Surface* geCreateSurface(int width, int height);
```

Удалить поверхность.

```c
void geDeleteSurface(GE_Surface* surface);
```

Загрузить файл изображения как поверхность. 

```c
GE_Surface* geLoadImage(const char* fileName);
```

Копировать часть поверхности surface в зону заданную прямоугольником dstRect из зоны заданной прямоугольником srcRect.
Если surface = NULL то будет использовать поверхность окна
Если dstRect = NULL то вывод будет по координатам 0, 0 в размер поверхности. 
Если srcRect = NULL то будет скопирована вся поверхность. 

```c
void geBlitSurface(GE_Surface* surface, const GE_Rect* dstRect, const GE_Rect* srcRect);
```

Разметить поверхность. Копировать содержимое поверхности из видеопамяти в обычную память и предоставить доступ к размеченным данным. 

Данные могут быть размечены в нескольких режимах доступа

GE_READ_ONLY - только чтение, измененные данные не будут записаны в поверхность при завершении разметки
GE_WRITE_ONLY - только запись, при разметки будет выделена память для данных но данные не будут скопированы из поверхности. Но будут записаны при завершении разметки.
GL_READ_WRITE - чтение и запись

Функция вернет указатель на данные.

```c
void* geMapSurface(GE_Surface* surface, int access);
```

Завершить разметку. 

```c
void geUnmapSurface(GE_Surface* surface);
```

Обновить разметку. 
Если поверхность размечена, данные будут помещены из памяти на поверхность но разметка не завершиться.

```c
void geUpdateSurface(GE_Surface* surface);
```

Вернет ширину поверхности

```c
int geGetSurfaceWidth(const GE_Surface* surface);
```

Вернет высоту поверхности

```c
int geGetSurfaceHeight(const GE_Surface* surface);
```

Вернет тип доступа размеченной поверхности либо GE_NONE если поверхность не размечена.

```c
int geGetSurfaceAccess(const GE_Surface* surface);
```

Вернет указатель на данные размеченной поверхности.

```c
void* geGetSurfacePtr(const GE_Surface* surface);
```

## Шрифты и вывод текста

Данные о изображении символа.

```c
typedef struct GE_Glyph {
	struct GE_Glyph* next;
	wchar_t character;
	int size;
	GE_Rect rect;
	GLuint texture;
	int advance;
} GE_Glyph;
```

Шрифт. 
Шрифт кеширует используемые глифы при для быстрого повторного использования. 

```c
typedef struct GE_Font {
	FT_Face face;
	int size;
	GE_Glyph* cache;	
} GE_Font;
```

Загрузить шрифт

```c
GE_Font* geLoadFont(const char* fileName, int size);
```

Удалить шрифт

```с
void geDeleteFont(GE_Font* font);
```

Изменить размер шрифта

```c
void GE_FontSize(GE_Font* font, int size);
```

Вернет размер шрифта

```c
int geGetFontSize(GE_Font* font);
```

Очистить кеш глифов шрифта

```c
void geClearFontCache(GE_Font* font);
```

Получить глиф для unicode символа 

```c
GE_Glyph* geGetGlyph(GE_Font* font, wchar_t character);
```

Нарисовать глиф

```c
void geDrawGlyph(GE_Glyph* glyph, int x, int y);
```

Напечатать unicode символ

```c
void geDrawUnicodeCharacter(GE_Font* font, int x, int y, wchar_t character);
```

Напечатать unicode строку

```c
void geDrawUnicodeString(GE_Font* font, int x, int y, const wchar_t* string);
````

Напечатать utf8 строку

```c
void geDrawUtf8String(GE_Font* font, int x, int y, const char* string)
```

# ToDo

- [*] Сохранять состояние движка в geBegin и восстанавливать в geEnd
- [*] Разметка поверхностей
- [*] Рендер на поверхность
- [*] Шрифты
- [ ] Блит с экрана если указано NULL
- [*] Рисование прямоугольников и линий
- [*] Понять почему FreeType2 половину символов рендерит криво - решение glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
- [ ] Создание поверхности из указателя
- [*] Печать utf8
- [ ] Найти замену iconv, так как это какойто пи....ц