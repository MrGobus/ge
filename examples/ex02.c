#include <ge.h>

#include <time.h>

/**
	Обработчик ошибки
	@param code - код ошибки
	@param message - сообщение об ошибке
*/

void errorCallback(GEint code, const GEchar* message) {
	fprintf(stderr, "error: %d\n%s\n", code, message);
	geTerminate();
	exit(code);
}

#define OBJECT_COUNT 100

#define OBJECT_MIN_SPEED 3

typedef struct Object {
	int x;
	int y;
	int speed;
} Object;

Object objects[OBJECT_COUNT];

int main() {
	geSetErrorCallback(errorCallback);
	if (geInit(640, 480, "Game Engine") == GE_OK) {
		GLFWwindow* window = geGetGLFWwindow();
		
		GEsurface* surface = geLoadSurface("image.png");

		for (int i = 0; i < OBJECT_COUNT; i++) {
			objects[i].x = rand() % (geGetScreenWidth() - geGetSurfaceWidth(surface));
			objects[i].y = rand() % (geGetScreenHeight() - geGetSurfaceHeight(surface));
			objects[i].speed = rand() % 10 + OBJECT_MIN_SPEED;
		}
		
		GErect dstRect = {
			0, 0,
			geGetSurfaceWidth(surface), geGetSurfaceHeight(surface)					
		};
		
		clock_t time = clock();
		
		GEfont* font = geLoadFont("OpenSans-Regular.ttf", 16);
		
		char fpsString[256];
		int fps = 0;

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			geBegin(GE_NULL);
				fps++;
				geSetColor(0.1, 0.1, 0.1, 1);
				geClear();
				
				for (int i = 0; i < OBJECT_COUNT; i++) {
					dstRect.x = objects[i].x;
					dstRect.y = objects[i].y;
					geBlitSurface(surface, &dstRect, GE_NULL);
					
					objects[i].y += objects[i].speed;
					if (objects[i].y > geGetScreenHeight()) {
						objects[i].x = rand() % (geGetScreenWidth() - geGetSurfaceWidth(surface));
						objects[i].y = - geGetSurfaceHeight(surface);
						objects[i].speed = rand() % 10 + OBJECT_MIN_SPEED;
					}
				}
				
				if (clock() > (time + 1000)) {
					time = clock();
					sprintf(fpsString, "fps: %d", fps);
					fps = 0;
				}
				
				geSetColor(1, 1, 1, 1);
				geDrawUtf8String(font, 10, 10, fpsString);
				
			geEnd();
			glfwSwapBuffers(window);
		}
		
		geDeleteFont(font);
		
		geDeleteSurface(surface);
		
		geTerminate();
	}
	printf("done\n");
	return 0;
}