#include <ge.h>

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

int main() {
	geSetErrorCallback(errorCallback);
	if (geInit(640, 480, "Game Engine") == GE_OK) {
		GLFWwindow* window = geGetGLFWwindow();
		GEfont* font = geLoadFont("OpenSans-Regular.ttf", 32);
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			geBegin(GE_NULL);
				geSetColor(0.1, 0.1, 0.1, 1);
				geClear();
				geSetColor(1, 0, 1, 1);
				geDrawUnicodeString(font, 20, 0, L"Привет мир form unicode");
				geSetColor(1, 1, 0, 1);
				geDrawUtf8String(font, 20, geGetFontSize(font), "Привет мир from uft8");
			geEnd();
			glfwSwapBuffers(window);
		}
		geDeleteFont(font);
		geTerminate();
	}
	printf("done\n");
	return 0;
}