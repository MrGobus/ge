int ge_errorCode = GE_OK; // Код ошибки
char* ge_errorMessage = GE_NULL; // Сообщение об ошибке
GEerrorCallback ge_errorCallback = GE_NULL; // Обработчик ошибки

/**
	Генерация ошибки. 
	Буде вызван обработчик ошибки если он установлен (не GE_NULL) и код ошибки не GE_OK.
	@param code - код ошибки 
	@param message - сообщение об ошибке, возможно GE_NULL
*/

void geError(int code, const char* message) {
	ge_errorCode = code;
	if (ge_errorMessage) {
		free(ge_errorMessage);
	}
	if (message) {
		ge_errorMessage = (char*)malloc(strlen(message) + 1);
		strcpy(ge_errorMessage, message);
	} else {
		ge_errorMessage = GE_NULL;
	}
	if (ge_errorCallback && code) {
		ge_errorCallback(ge_errorCode, ge_errorMessage);
	}
}

/**
	@return код ошибки
*/

int geGetErrorCode() {
	return ge_errorCode;	
}

/**
	@return сообщение об ошибке
*/

const char* geGetErrorMessage() {
	return ge_errorMessage;
}

/**
	Установить обработчик ошибки
	@paran callback - обработчик ошибки
*/

void geErrorCallback(GEerrorCallback callback) {
	ge_errorCallback = callback;
}

/**
	@return обработчик ошибки
*/

GEerrorCallback geGetErrorCallback() {
	return ge_errorCallback;
}