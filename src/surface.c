/**
	Создать поверхность
	@param width - ширина
	@param height - высота
*/

GEsurface* geCreateSurface(GEint width, GEint height) {
	GEsurface* surface = (GEsurface*)malloc(sizeof(GEsurface));
	surface->width = width;
	surface->height = height;
	glGenTextures(1, &surface->texture);
	glBindTexture(GL_TEXTURE_2D, surface->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, GE_NULL);
	surface->ptr = GE_NULL;
	surface->access = GE_NONE;
	surface->usageCounter = 1;
	return surface;	
}

/**
	Загрузить поверхность из файла изображения
*/

GEsurface* geLoadImage(const GEchar* fileName) {
	ILuint image;
	ilGenImages(1, &image);
	if (ilLoadImage(fileName)) {
		GEsurface* surface = (GEsurface*)malloc(sizeof(GEsurface));
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		surface->width = ilGetInteger(IL_IMAGE_WIDTH);
		surface->height = ilGetInteger(IL_IMAGE_HEIGHT);
		glGenTextures(1, &surface->texture);
		glBindTexture(GL_TEXTURE_2D, surface->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->width, surface->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());
		ilDeleteImages(1, &image);
		surface->ptr = GE_NULL;
		surface->access = GE_NONE;
		surface->usageCounter = 1;
		return surface;	
	} else  {
		ilDeleteImages(1, &image);
		return GE_NULL;		
	}	
}

/**
	Удалить поверхность
*/

void geDeleteSurface(GEsurface* surface) {
	surface->usageCounter--;
	if (surface->usageCounter) {
		return;
	}
	if (surface->ptr) {
		free(surface->ptr);
	}
	glDeleteTextures(1, &surface->texture);
	free(surface);
}

/**
	Рисует заданную часть поверхности
	@param surface - поверхность
	@param dstRect - куда (если GE_NULL то вывод по координатам 0,0 в размер изображения)
	@param dstRect - откуда (если GE_NULL используется все изображение)
*/

void geBlitSurface(GEsurface* surface, const GErect* dstRect, const GErect* srcRect) {
	GLfloat verticesData[8];
	GLfloat texCoordData[8];
	
	glUniform1i(ge_shader.uniformLocation.enableTexture, 1);
	
	if (dstRect) {
		verticesData[0] = dstRect->x;
		verticesData[1] = dstRect->y;

		verticesData[2] = dstRect->x;
		verticesData[3] = dstRect->y + dstRect->height;
		
		verticesData[4] = dstRect->x + dstRect->width;
		verticesData[5] = dstRect->y;
		
		verticesData[6] = dstRect->x + dstRect->width;
		verticesData[7] = dstRect->y + dstRect->height;
	} else {
		if (srcRect) {
			verticesData[0] = 0;
			verticesData[1] = 0;

			verticesData[2] = 0;
			verticesData[3] = srcRect->height;
			
			verticesData[4] = srcRect->width;
			verticesData[5] = 0;
			
			verticesData[6] = srcRect->width;
			verticesData[7] = srcRect->height;
		} else {
			verticesData[0] = 0;
			verticesData[1] = 0;

			verticesData[2] = 0;
			verticesData[3] = surface->height;
			
			verticesData[4] = surface->width;
			verticesData[5] = 0;
			
			verticesData[6] = surface->width;
			verticesData[7] = surface->height;
		}
	}
	
	if (srcRect) {
		GLfloat aw = 1.0 / surface->width;
		GLfloat ah = 1.0 / surface->height;
		GLfloat tx = srcRect->x * aw;
		GLfloat ty = srcRect->y * ah;
		GLfloat tw = srcRect->width * aw;
		GLfloat th = srcRect->height * ah;
		
		texCoordData[0] = tx;
		texCoordData[1] = ty;
		
		texCoordData[2] = tx;
		texCoordData[3] = ty + th;
		
		texCoordData[4] = tx + tw;
		texCoordData[5] = ty;
		
		texCoordData[6] = tx + tw;
		texCoordData[7] = ty + th;
	} else {
		texCoordData[0] = 0;
		texCoordData[1] = 0;
		
		texCoordData[2] = 0;
		texCoordData[3] = 1;
		
		texCoordData[4] = 1;
		texCoordData[5] = 0;
		
		texCoordData[6] = 1;
		texCoordData[7] = 1;
	}
	
	glBindTexture(GL_TEXTURE_2D, surface->texture);
	
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesData), verticesData);
	
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.texCoordBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoordData), texCoordData);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/**
	Копирует данные поверхности в память и возвращает указатель на них
	@param surface - поверхности
	@param access - тип доступа (GE_READ_ONLY, GE_WRITE_ONLY, GE_READ_WRITE)
*/

void* geMapSurface(GEsurface* surface, int access) {
	surface->access = access;
	if (!surface->ptr) {
		surface->ptr = (void*)malloc(surface->width * surface->height * 4);
	}
	if (access != GE_WRITE_ONLY) {
		glBindTexture(GL_TEXTURE_2D, surface->texture);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->ptr);
	}
	return surface->ptr;
}

/**
	Завершает разметку. Копирует размеченные данные в поверхность и освобождает выделенную память
	@param surface - поверхность
*/

void geUnmapSurface(GEsurface* surface) {
	if (surface->access != GE_READ_ONLY) {
		glBindTexture(GL_TEXTURE_2D, surface->texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_BGRA, GL_UNSIGNED_BYTE, surface->ptr);
	}
	free(surface->ptr);
	surface->ptr = GE_NULL;
}

/**
	Обновляет поверхность данными разметки.
	@param surface - поверхность
*/

void geUpdateSurface(GEsurface* surface) {
	if (surface->access != GE_READ_ONLY) {
		glBindTexture(GL_TEXTURE_2D, surface->texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_BGRA, GL_UNSIGNED_BYTE, surface->ptr);
	}
}

/**
	@param surface - поверхность
	@return ширина
*/

GEint geGetSurfaceWidth(const GEsurface* surface) {
	return surface->width;	
}

/**
	@param surface - поверхность
	@return высота
*/

GEint geGetSurfaceHeight(const GEsurface* surface) {
	return surface->height;
}

/**
	@param surface - поверхность
	@return тип разметки
*/

GEint geGetSurfaceAccess(const GEsurface* surface) {
	return surface->access;
}

/**
	@param surface - поверхность
	@return указатель на размеченный данные
*/

GEvoid* geGetSurfacePtr(const GEsurface* surface) {
	return surface->ptr;
}
