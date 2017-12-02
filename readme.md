# 2D Graphic Engine GE

# Список функций

## Обработка ошибок

Тип для обработчика ошибок.

```c
typedef void (*GEerrorCallback) (GEint, const GEchar*)
```

Генерация ошибки. 
Параметры код ошибки и сообщение. Вызовет обработчик ошибки если он установлен.

```c
void geError(GEint code, const GEchar* message);
```

Получить код последней ошибки.

```c
GEint geGetErrorCode();
```

```
GE_NONE - нет ошибки
GE_ERROR - ошибка
```

Получить сообщение об ошибке.

```c
const GEchar* geGetErrorMessage();
```

Установить обработчик ошибок.

```c
void geSetErrorCallback(GEerrorCallback callback);
```

Получить обработчик ошибок.

```c
GEerrorCallback geGetErrorCallback();
```

## Инициализация

Инициализация библиотеки.
Параметры - размер окна и заголовок.

```c
GEint geInit(GEint width, GEint height, const GEchar* title);
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
typedef struct GErect {
	GEint x;
	GEint y;
	GEint width;
	GEint height;
} GErect;
```

## Рисование

Начать процесс рисования. Если surface == GE_NULL вывод будет в окно иначе в заданную поверхность.

```c
void geBegin(GEsurface* surface);
```

Завершить процесс рисования

```c
void geEnd();
```

Вернет ширину окна

```c
GEint geGetWindowWidth();
```

Вернет высоту окна

```c
GEint geGetWindowHeight();
```

Вернет ширину экрана - окна либо поверхности на которую производится рендер.

```c
GEint geGetScreenWidth();
```

Вернет высоту экрана - окна либо поверхности на которую производится рендер.

```c
GEint geGetScreenHeight();
```

Установить глобальную прозрачность.

```c
void geSetOpacity(GEfloat opacity);
```

Вернет глобальную прозрачность

```c
GEfloat geGetOpacity();
```

Установить цвет закраски (прямоугольники, линии, текст, очистка экрана)

```c
void geSetColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
```

Получить цвет закраски

```c
void geGetColor(GEfloat* color);
```

Очистить экран

```c
void geClear();
```

## Поверхность

Поверхность, это прямоугольная область хранящая графические данные. Вся поверхность либо ее часть может быть скопирована на экран либо на другую поверхность.

Тип поверхности.

```c
typedef struct GEsurface {
	GLuint texture;
	GEint width;
	GEint height;
	GEint access;
	GEvoid* ptr;
	GEint usageCounter;
} GEsurface;
```

Создать поверхность заданного размера.

```c
GEsurface* geCreateSurface(GEint width, GEint height);
```

Удалить поверхность.

```c
void geDeleteSurface(GEsurface* surface);
```

Загрузить файл изображения как поверхность. 

```c
GEsurface* geLoadSurface(const GEchar* fileName);
```

Копировать часть поверхности surface в зону заданную прямоугольником dstRect из зоны заданной прямоугольником srcRect.
Если surface = GE_NULL то будет использовать поверхность окна
Если dstRect = GE_NULL то вывод будет по координатам 0, 0 в размер поверхности. 
Если srcRect = GE_NULL то будет скопирована вся поверхность. 

```c
void geBlitSurface(GEsurface* surface, const GErect* dstRect, const GErect* srcRect);
```

Разметить поверхность. Копировать содержимое поверхности из видеопамяти в обычную память и предоставить доступ к размеченным данным. 

Данные могут быть размечены в нескольких режимах доступа

GE_READ_ONLY - только чтение, измененные данные не будут записаны в поверхность при завершении разметки
GE_WRITE_ONLY - только запись, при разметки будет выделена память для данных но данные не будут скопированы из поверхности. Но будут записаны при завершении разметки.
GL_READ_WRITE - чтение и запись

Функция вернет указатель на данные.

```c
void* geMapSurface(GEsurface* surface, int access);
```

Завершить разметку. 

```c
void geUnmapSurface(GEsurface* surface);
```

Обновить разметку. 
Если поверхность размечена, данные будут помещены из памяти на поверхность но разметка не завершиться.

```c
void geUpdateSurface(GEsurface* surface);
```

Вернет ширину поверхности

```c
GEint geGetSurfaceWidth(const GEsurface* surface);
```

Вернет высоту поверхности

```c
GEint geGetSurfaceHeight(const GEsurface* surface);
```

Вернет тип доступа размеченной поверхности либо GE_NONE если поверхность не размечена.

```c
GEint geGetSurfaceAccess(const GEsurface* surface);
```

Вернет указатель на данные размеченной поверхности.

```c
GEvoid* geGetSurfacePtr(const GEsurface* surface);
```

## Шрифты и вывод текста

Данные о изображении символа.

```c
typedef struct GEglyph {
	struct GEglyph* next;
	GEunicodeCharacter character;
	GEint size;
	GErect rect;
	GLuint texture;
	GEint advance;
} GEglyph;
```

Шрифт. 
Шрифт кеширует используемые глифы при для быстрого повторного использования. 

```c
typedef struct GEfont {
	FT_Face face;
	GEint size;
	GEglyph* cache;	
} GEfont;
```

Загрузить шрифт

```c
GEfont* geLoadFont(const GEchar* fileName, GEint size);
```

Удалить шрифт

```с
void geDeleteFont(GEfont* font);
```

Изменить размер шрифта

```c
void geSetFontSize(GEfont* font, GEint size);
```

Вернет размер шрифта

```c
GEint geGetFontSize(GEfont* font);
```

Очистить кеш глифов шрифта

```c
void geClearFontCache(GEfont* font);
```

Получить глиф для unicode символа 

```c
GEglyph* geGetGlyph(GEfont* font, GEunicodeCharacter character);
```

Нарисовать глиф

```c
void geDrawGlyph(GEglyph* glyph, GEint x, GEint y);
```

Напечатать unicode символ

```c
void geDrawUnicodeCharacter(GEfont* font, GEint x, GEint y, GEunicodeCharacter character);
```

Напечатать unicode строку

```c
void geDrawUnicodeString(GEfont* font, GEint x, GEint y, const GEunicodeCharacter* string);
````

# ToDo

- [*] Сохранять состояние движка в geBegin и восстанавливать в geEnd
- [*] Разметка поверхностей
- [*] Рендер на поверхность
- [*] Шрифты
- [ ] Блит с экрана если указано GE_NULL
- [ ] Рисование прямоугольников и линий
- [*] Понять почему FreeType2 половину символов рендерит криво - решение glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
- [ ] Создание поверхности из указателя
- [ ] Печать utf8