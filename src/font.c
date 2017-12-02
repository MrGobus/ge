/**
	Создать шрифт
	@param fileName - имя файла шрифта
	@param size - размер шрифта
	@return шрифт или GE_NULL
*/

GEfont* geLoadFont(const GEchar* fileName, GEint size) {
	FT_Face ft_face;
	FT_Error ft_error = FT_New_Face(ge_ft_library, fileName, 0, &ft_face);
	if (ft_error) {
		geError(GE_ERROR, "FT_New_Face error");
		return GE_NULL;
	}
	FT_Set_Pixel_Sizes(ft_face, 0, size);
	GEfont* font = (GEfont*)malloc(sizeof(GEfont));
	font->size = size;
	font->cache = GE_NULL;	
	font->face = ft_face;
	return font;
}

/**
	Очистка кеша глифов шрифта
	@param font - шрифт
*/

void geClearFontCache(GEfont* font) {
	while (font->cache) {
		GEglyph* glyph = font->cache;
		font->cache = glyph->next;
		glDeleteTextures(1, &glyph->texture);
		free(glyph);
	}
}

/**
	Удалить шрифт
	@param font - шрифт
*/

void geDeleteFont(GEfont* font) {
	FT_Done_Face(font->face);
	geClearFontCache(font);
	free(font);	
}

inline GEuint nextpow2(GEuint n) {
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return ++n;
}

/**
	Получить глиф символа
	@param font - шрифт
	@param character - unicode код символа
	@return глиф или GE_NULL
*/

GEglyph* geGetGlyph(GEfont* font, GEunicodeCharacter character) {
	FT_Error ft_error;
	
	GEglyph* glyph = font->cache;
	while (glyph) {
		if ((glyph->character == character) && (glyph->size == font->size)) {
			return glyph;
		}
		glyph = glyph->next;
	}
	
	FT_UInt ft_glyphIndex = FT_Get_Char_Index(font->face, character);
	if (!ft_glyphIndex) {
		return GE_NULL;
	}
	
	ft_error = FT_Load_Glyph(font->face, ft_glyphIndex, FT_LOAD_DEFAULT);
	if (ft_error) {
		return GE_NULL;
	}
	
	ft_error = FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL);
	if (ft_error) {
		return GE_NULL;
	}
	
	glyph = (GEglyph*)malloc(sizeof(GEglyph));
	glyph->next = font->cache;
	font->cache = glyph;
	
	glyph->character = character;
	glyph->size = font->size;
	
	FT_GlyphSlot slot = font->face->glyph;
	
	glyph->rect.x = slot->bitmap_left;
	glyph->rect.y = font->size - slot->bitmap_top;
	glyph->rect.width = slot->bitmap.pitch;
	glyph->rect.height = slot->bitmap.rows;
	glyph->advance = slot->advance.x >> 6;
	
	glGenTextures(1, &glyph->texture);
	glBindTexture(GL_TEXTURE_2D, glyph->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph->rect.width, glyph->rect.height, 0, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
	
	GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		
	return glyph;
}

/**
	Рисует глиф
	@param glyph - глиф
	@param x - x
	@param y - y
*/

void geDrawGlyph(GEglyph* glyph, GEint x, GEint y) {
	glUniform1i(ge_shader.uniformLocation.enableTexture, 1);
	glUniform1i(ge_shader.uniformLocation.fontMode, 1);

	GEint _x = x + glyph->rect.x;
	GEint _y = y + glyph->rect.y;
	
	const GLfloat verticesData[8] = {
		_x, _y,
		_x, _y + glyph->rect.height,
		_x + glyph->rect.width, _y,
		_x + glyph->rect.width, _y + glyph->rect.height
	};
	
	const GLfloat texCoordData[8] = {
		0, 0,
		0, 1,
		1, 0,
		1, 1
	};
	
	glBindTexture(GL_TEXTURE_2D, glyph->texture);
	
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.verticesBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesData), verticesData);
	
	glBindBuffer(GL_ARRAY_BUFFER, ge_rect.texCoordBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(texCoordData), texCoordData);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUniform1i(ge_shader.uniformLocation.fontMode, 0);
}

/**
	Рисует unicode символ на экране
	@param font - шрифт
	@param x - x
	@param y - y
	@param character - unicode код символа
*/

void geDrawUnicodeCharacter(GEfont* font, GEint x, GEint y, GEunicodeCharacter character) {
	GEglyph* glyph = geGetGlyph(font, character);
	if (glyph) {
		geDrawGlyph(glyph, x, y);
	}
}

/**
	Выводит unicode строку 
	@param font - шрифт
	@param x - x
	@param y - y
	@param string - unicode строка
*/

void geDrawUnicodeString(GEfont* font, GEint x, GEint y, const GEunicodeCharacter* string) {
	GEint cursorX = x;
	GEint cursorY = y;
	const GEunicodeCharacter* character = string;
	while (*character) {
		GEglyph* glyph = geGetGlyph(font, *character);
		if (glyph) {
			geDrawGlyph(glyph, cursorX, cursorY);
			cursorX += glyph->advance;
		}
		character++;
	}
}

/**
	Выводит unicode строку 
	@param font - шрифт
	@param x - x
	@param y - y
	@param string - unicode строка
*/

void geDrawUtf8String(GEfont* font, GEint x, GEint y, const GEchar* string) {
	size_t stringLength = strlen(string);
	size_t stringLengthCopy = stringLength;
	size_t utf8bufferLength = stringLength * sizeof(GEunicodeCharacter);
	GEchar* buffer = (GEchar*)malloc(stringLength + 1);
	strcpy(buffer, string);
	GEunicodeCharacter* utf8buffer = (GEunicodeCharacter*)malloc(utf8bufferLength + sizeof(GEchar));
	GEchar* utf8bufferStart = (char*)utf8buffer;
	GEchar* stringBufferStart = buffer;
	if (ge_utf8ToUnicode) {
		iconv(ge_utf8ToUnicode, &stringBufferStart, &stringLength, &utf8bufferStart, &utf8bufferLength);
	}
	utf8buffer[stringLengthCopy] = 0;
	geDrawUnicodeString(font, x, y, utf8buffer);
	free(utf8buffer);
	free(buffer);
}

/**
	Установить размер шрифта
	@param font - шрифт
	@param size - размер
*/

void geSetFontSize(GEfont* font, GEint size) {
	FT_Set_Pixel_Sizes(font->face, 0, size);
	font->size = size;
}

/**
	@param font - шрифт
	@return размер шрифта
*/

GEint geGetFontSize(GEfont* font) {
	return font->size;
}