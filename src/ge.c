#include "ge.h"

#include "error.c"

GEfloat ge_color[4];

FT_Library ge_ft_library = 0;

typedef struct GEstate {
	struct GEstate* next;
	GEsurface* surface;
	GEfloat opacity;
	GEfloat clearColor[4];
} GEstate;

GEstate* ge_stateStack = GE_NULL;

GEint ge_ilInitStatus = 0;
GEint ge_FreeTypeInitState = 0;

GLFWwindow* ge_window = GE_NULL; // Окно GLFW3

const GLchar* vertexShaderSource[] = {
	"#version 330\n",
	"layout (location = 0) in vec2 vertexPosition;\n",
	"layout (location = 1) in vec2 vertexTexCoord;\n",
	"uniform mat4 projectionMatrix;\n",
	"out vec2 fragTexCoord;\n",
	"void main() {\n",
		"fragTexCoord = vertexTexCoord;\n",
		"gl_Position = projectionMatrix * vec4(vertexPosition, 0, 1);\n",
	"}"
};

const GLchar* fragmentShaderSource[] = {
	"#version 330\n",
	"in vec2 fragTexCoord;\n",
	"uniform sampler2D textureSampler;\n",
	"uniform int enableTexture;\n",
	"uniform vec4 color;\n",
	"uniform float opacity;\n",
	"uniform int fontMode;\n",
	"out vec4 fragColor;\n",
	"void main() {\n",
		"if (enableTexture) {\n",
			"if (fontMode) {\n",
				"fragColor = color * texture(textureSampler, fragTexCoord);\n",
				
			"} else {\n",
				"fragColor = texture(textureSampler, fragTexCoord);\n",
			"}\n",
		"} else {\n",
			"fragColor = color;\n",
		"}\n",
		"fragColor.a *= opacity;\n",
	"}"
};

GEint ge_screenWidth = 0;
GEint ge_screenHeight = 0;

struct {
	GLuint program;
	struct UniformLocation {
		GLint projectionMatrix;
		GLint textureSampler;
		GLint enableTexture;
		GLint color;
		GLint opacity;
		GLint fontMode;
	} uniformLocation;
} ge_shader = {
	0, // program
	{ // uniformLocation
		-1, // projectionMatrix
		-1, // textureSampler
		-1, // color
		-1, // enableTexture
		-1, // opacity
		-1 // fontMode - color.a *= texuter.r
	}
};

struct {
	GLuint vertexArray;
	GLuint verticesBuffer;
	GLuint texCoordBuffer;	
} ge_rect = {
	0, // vertexArray
	0, // verticesBuffer
	0 // texCoordBuffer
};

GLuint ge_framebuffer = 0;

/** 
	@retrun окно GLFW3
*/

GLFWwindow* geGetGLFWwindow() {
	return ge_window;
}

/**
	Обработчик ошибки GLFW3
	@param code - код ошибки
	@param message - сообщение об ошибке
*/

void ge_glfwErrorCallback(int code, const char* message) {
	geError(GE_ERROR, message);	
}

#include "draw.c"
#include "surface.c"
#include "font.c"

/**
	Инициализация игрового движка
	@retrun статус выполнения
*/

GEint geInit(GEint width, GEint height, const GEchar* title) {
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLint status;
	GLint infoLogLength;
	
	ge_screenWidth = width;
	ge_screenHeight = height;

	// GLFW3
	glfwSetErrorCallback(ge_glfwErrorCallback);
	if (glfwInit() == GLFW_FALSE) {
		// geError вызывается из обработчика ge_glfwErrorCallback
		goto finalizeError;
	}

	// GLFWwindow
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	ge_window = glfwCreateWindow(width, height, title, 0, 0);
	if (ge_window == GE_NULL)  {
		// geError вызывается из обработчика ge_glfwErrorCallback
		goto finalizeError;
	}
	glfwGetWindowSize(ge_window, &ge_screenWidth, &ge_screenHeight);
	glfwMakeContextCurrent(ge_window);
	
	// GLEW
	glewExperimental = GE_TRUE;
	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK) {
		geError(GE_ERROR, (char*)glewGetErrorString(glewInitResult));
		goto finalizeError;		
	}
	
	// DevIL
	ilInit();
	if (ilGetError()) {
		ge_ilInitStatus = 0;
		geError(GE_ERROR, (char*)glewGetErrorString(glewInitResult));
		goto finalizeError;
	} else {
		ge_ilInitStatus = 1;
	}
	
	// FreeType 2
	FT_Error ft_error = FT_Init_FreeType(&ge_ft_library);
	if (ft_error) {
		geEnd(GE_ERROR, "FT_Init error");
		goto finalizeError;
	} else {
		ge_FreeTypeInitState = 1;
	}
	
	// Shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, sizeof(vertexShaderSource) / sizeof(vertexShaderSource[0]), vertexShaderSource, 0);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		ge_errorCode = GE_ERROR;
		if (ge_errorMessage) {
			free(ge_errorMessage);
		}
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		ge_errorMessage = (GEchar*)malloc(infoLogLength + 1);
		glGetShaderInfoLog(vertexShader, infoLogLength, 0, ge_errorMessage);
		ge_errorMessage[infoLogLength] = 0;
		if (ge_errorCallback) {
			ge_errorCallback(ge_errorCode, ge_errorMessage);
		}
	}
	
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, sizeof(fragmentShaderSource) / sizeof(fragmentShaderSource[0]), fragmentShaderSource, 0);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status) {
		ge_errorCode = GE_ERROR;
		if (ge_errorMessage) {
			free(ge_errorMessage);
		}
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		ge_errorMessage = (GEchar*)malloc(infoLogLength + 1);
		glGetShaderInfoLog(fragmentShader, infoLogLength, 0, ge_errorMessage);
		ge_errorMessage[infoLogLength] = 0;
		if (ge_errorCallback) {
			ge_errorCallback(ge_errorCode, ge_errorMessage);
		}
	}
	
	ge_shader.program = glCreateProgram();
	glAttachShader(ge_shader.program, vertexShader);
	glAttachShader(ge_shader.program, fragmentShader);
	glLinkProgram(ge_shader.program);
	glGetProgramiv(ge_shader.program, GL_LINK_STATUS, &status);
	if (!status) {
		ge_errorCode = GE_ERROR;
		if (ge_errorMessage) {
			free(ge_errorMessage);
		}
		glGetProgramiv(ge_shader.program, GL_INFO_LOG_LENGTH, &infoLogLength);
		ge_errorMessage = (GEchar*)malloc(infoLogLength + 1);
		glGetProgramInfoLog(ge_shader.program, infoLogLength, 0, ge_errorMessage);
		ge_errorMessage[infoLogLength] = 0;
		if (ge_errorCallback) {
			ge_errorCallback(ge_errorCode, ge_errorMessage);
		}
	}
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glUseProgram(ge_shader.program);
	
	ge_shader.uniformLocation.projectionMatrix = glGetUniformLocation(ge_shader.program, "projectionMatrix");
	ge_shader.uniformLocation.textureSampler = glGetUniformLocation(ge_shader.program, "textureSampler");	
	ge_shader.uniformLocation.enableTexture = glGetUniformLocation(ge_shader.program, "enableTexture");	
	ge_shader.uniformLocation.color = glGetUniformLocation(ge_shader.program, "color");	
	ge_shader.uniformLocation.opacity = glGetUniformLocation(ge_shader.program, "opacity");
	ge_shader.uniformLocation.fontMode = glGetUniformLocation(ge_shader.program, "fontMode");
	
	glUniform1i(ge_shader.uniformLocation.enableTexture, 1);
	glUniform4f(ge_shader.uniformLocation.color, 1, 1, 0, 1);
	glUniform1f(ge_shader.uniformLocation.opacity, 1);
	glUniform1i(ge_shader.uniformLocation.textureSampler, 0);
	glUniform1i(ge_shader.uniformLocation.fontMode, 0);
	
	// ge_rect
	glGenVertexArrays(1, &ge_rect.vertexArray);
	glBindVertexArray(ge_rect.vertexArray);
	
	glGenBuffers(1, &ge_rect.verticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 16, GE_NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	glGenBuffers(1, &ge_rect.texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 16, GE_NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	
	// Framebuffer
	glGenFramebuffers(1, &ge_framebuffer);
	
	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return GE_OK;
	
	finalizeError: {
		if (vertexShader) {
			glDeleteShader(vertexShader);
		}
		if (fragmentShader) {
			glDeleteShader(fragmentShader);
		}
		geTerminate();
		return GE_ERROR;
	}
}

/**
	Завершение работы игрового движка
*/

void geTerminate() {
	if (ge_framebuffer) {
		glDeleteFramebuffers(1, &ge_framebuffer);
		ge_framebuffer = 0;
	}
	if (ge_rect.vertexArray) {
		glDeleteVertexArrays(1, &ge_rect.vertexArray);
		ge_rect.vertexArray = 0;
	}
	if (ge_rect.verticesBuffer) {
		glDeleteBuffers(1, &ge_rect.verticesBuffer);
		ge_rect.verticesBuffer = 0;
	}
	if (ge_rect.texCoordBuffer) {
		glDeleteBuffers(1, &ge_rect.texCoordBuffer);
		ge_rect.texCoordBuffer = 0;
	}
	if (ge_shader.program) {
		glDeleteProgram(ge_shader.program);
		ge_shader.program = 0;
	}
	if (ge_FreeTypeInitState) {
		FT_Done_FreeType(ge_ft_library);
		ge_FreeTypeInitState = 0;		
	}
	if (ge_ilInitStatus) {
		ilShutDown();
		ge_ilInitStatus = 0;		
	}
	if (ge_window) {
		glfwDestroyWindow(ge_window);
		ge_window = GE_NULL;
	}
	glfwTerminate();	
}

/**
	Устанавливает состояние
	@param state - состояние
*/

void ge_setState(GEstate* state) {
	glUseProgram(ge_shader.program);
	glBindVertexArray(ge_rect.vertexArray);
	
	if (state->surface) {
		glBindFramebuffer(GL_FRAMEBUFFER, ge_framebuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, state->surface->texture, 0);
		
		GLenum drawBuffers[1] = {
			GL_COLOR_ATTACHMENT0
		};
		glDrawBuffers(1, drawBuffers);

		ge_screenWidth = state->surface->width;
		ge_screenHeight = state->surface->height;
		
		const GLfloat projectionMatrix[16] = {
			2.0 / ge_screenWidth, 0, 0, 0,
			0, 2.0 / ge_screenHeight, 0, 0,
			0, 0, 1, 0,
			-1, -1, 0, 1
		};
		glUniformMatrix4fv(ge_shader.uniformLocation.projectionMatrix, 1, GL_FALSE, projectionMatrix);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		
		glfwGetWindowSize(ge_window, &ge_screenWidth, &ge_screenHeight);

		const GLfloat projectionMatrix[16] = {
			2.0 / ge_screenWidth, 0, 0, 0,
			0, -2.0 / ge_screenHeight, 0, 0,
			0, 0, 1, 0,
			-1, 1, 0, 1
		};
		glUniformMatrix4fv(ge_shader.uniformLocation.projectionMatrix, 1, GL_FALSE, projectionMatrix);
	}
	
	glViewport(0, 0, ge_screenWidth, ge_screenHeight);
	
	glUniform1f(ge_shader.uniformLocation.opacity, state->opacity);
	
	glClearColor(state->clearColor[0], state->clearColor[1], state->clearColor[2], state->clearColor[3]);
}

/**
	Начать процесс рисования
	@param surface - поверхность на которую нужно выводить изображение
*/

void geBegin(GEsurface* surface) {
	GEstate* state = (GEstate*)malloc(sizeof(GEstate));
	state->next = ge_stateStack;
	ge_stateStack = state->next;
	state->surface = surface;
	if (surface) {
		surface->usageCounter++;
	}
	
	glGetUniformfv(ge_shader.program, ge_shader.uniformLocation.opacity, &state->opacity);
	
	glGetFloatv(GL_COLOR_CLEAR_VALUE, state->clearColor);
	
	ge_setState(state);
}

/**
	Завершить процесс рисования
*/
	
void geEnd() {
	GEstate* state = ge_stateStack;
	if (state) {
		ge_stateStack = state->next;
		ge_setState(state);
		if (state->surface) {
			geDeleteSurface(state->surface);
		}
		free(state);
	}
}

/**
	Установить значение прозрачности
	@param opacity - прозрачность
*/

void geSetOpacity(GEfloat opacity) {
	glUniform1f(ge_shader.uniformLocation.opacity, opacity);
}

/**
	@retrun прозрачность
*/

GEfloat geGetOpacity() {
	GLfloat opacity;
	glGetUniformfv(ge_shader.program, ge_shader.uniformLocation.opacity, &opacity);
	return opacity;	
}

/**
	Очистить экран
*/

void geClear() {
	glClearColor(ge_color[0], ge_color[1], ge_color[2], ge_color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

/**
	@retrun ширина экрана
*/

GEint geGetScreenWidth() {
	return ge_screenWidth;
}

/**
	@retrun высота экрана
*/

GEint geGetScreenHeight() {
	return ge_screenHeight;
}

/**
	Установить цвет рисования
	@param r - r
	@param g - g
	@param b - b
	@param a - a
*/

void geSetColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	ge_color[0] = r;
	ge_color[1] = g;
	ge_color[2] = b;
	ge_color[3] = a;
	glUniform4f(ge_shader.uniformLocation.color, r, g, b, a);
}

/**
	Получить цвет рисования
	@param color - указатель куда записать цвет
*/

void geGetColor(GEfloat* color) {
	memcpy(color, ge_color, sizeof(GEfloat) * 4);
}