/**
	Рисует прямоугольник
	@param rect - данные прямоугольника
*/

void geRect(const GErect* rect) {
	glUniform1i(ge_shader.uniformLocation.enableTexture, 0);

	GLfloat verticesData[8] = {
		rect->x,
		rect->y,

		rect->x,
		rect->y + rect->height,

		rect->x + rect->width,
		rect->y + rect->height,

		rect->x + rect->width,
		rect->y
	};

	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesData), verticesData);
	
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

/**
	Рисует закрашенный прямоугольник
	@param rect - данные прямоугольника
*/

void geFillRect(const GErect* rect) {
	glUniform1i(ge_shader.uniformLocation.enableTexture, 0);

	GLfloat verticesData[8] = {
		rect->x,
		rect->y,
		rect->x,
		rect->y + rect->height,
		rect->x + rect->width,
		rect->y,
		rect->x + rect->width,
		rect->y + rect->height
	};

	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesData), verticesData);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/**
	Рисует линию
	@param x1 - x1
	@param y1 - y1
	@param x2 - x2
	@param y2 - y2
*/

void geLine(GLint x1, GLint y1, GLint x2, GLint y2) {
	glUniform1i(ge_shader.uniformLocation.enableTexture, 0);

	GLfloat verticesData[8] = {
		x1,	y1,
		x2,	y2
	};

	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesData), verticesData);
	
	glDrawArrays(GL_LINES, 0, 2);
}