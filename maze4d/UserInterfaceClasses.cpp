#include <UserInterfaceClasses.h>

//renders given text inside buffer with MaxWidth/MaxHeight parameters
void UserInterfaceItem::RenderRectangle(uint8_t* buffer, int posX, int posY, int width, int height, glm::u8vec3 color)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	int xGlobalOffset = posX;
	int yGlobalOffset = posY;
	int maxPixPos = 0;

	int maxPosShouldBe = maxWidth*maxHeight * 3 - 1;

	for (int rows = 0; rows < height; rows++)
		for (int xi = 0; xi < width; xi++)
		{
			int pixPos = (rows + yGlobalOffset) * maxWidth * 3 + (xi + xGlobalOffset) * 3;

			if (pixPos < maxWidth*maxHeight * 3 &&
				pixPos > 0)
			{
				
				if (pixPos > maxPixPos) maxPixPos = pixPos;
				float alpha = 1.0f; //no opacity

				buffer[pixPos] = color.x;// *alpha + buffer[pixPos] * (1 - alpha);
				buffer[pixPos + 1] = color.y;// *alpha + buffer[pixPos + 1] * (1 - alpha);
				buffer[pixPos + 2] = color.z;// *alpha + buffer[pixPos + 2] * (1 - alpha);
			}
		}
	
	maxPixPos = maxPixPos;
}

void UserInterfaceItem::RenderUItext(std::string text, int fontSize, uint8_t* buffer, int posX, int posY, glm::u8vec3 color)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	FT_Library ft;
	FT_Face face;

	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	if (FT_New_Face(ft, "fonts/courbd.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glm::u8vec3 redPixel(255, 0, 0);
	glm::u8vec3 whitePixel(255, 255, 255);
	glm::u8vec3 blackPixel(0, 0, 0);

	//glm::vec3 color = whitePixel;

	int xGlobalOffset = posX;
	int yGlobalOffset = posY;

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		char letter = *c;
		if (FT_Load_Char(face, *c, FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

		FT_GlyphSlot ch = face->glyph;

		int yOffset = yGlobalOffset + ch->bitmap_top - ch->bitmap.rows; //calculate up/down y offset for such letters as 'g'

		for (int rows = 0; rows < (int)ch->bitmap.rows; rows++)
			for (int xi = 0; xi < (int)ch->bitmap.width; xi++)
				if (rows + yOffset < (int)maxHeight &&
					rows + yOffset > 0 &&
					xi + xGlobalOffset < (int)maxWidth &&
					xi + xGlobalOffset > 0)
				{
					int glyphPixGray = ch->bitmap.buffer[((ch->bitmap.rows - rows - 1) * ch->bitmap.pitch + xi)];
					int pixPos = (rows + yOffset) * maxWidth * 3 + (xi + xGlobalOffset) * 3;

					float alpha = (float)glyphPixGray / (float)ch->bitmap.num_grays;

					buffer[pixPos] = (unsigned int)round(color.x * alpha + buffer[pixPos] * (1 - alpha));
					buffer[pixPos + 1] = (unsigned int)round(color.y * alpha + buffer[pixPos + 1] * (1 - alpha));
					buffer[pixPos + 2] = (unsigned int)round(color.z * alpha + buffer[pixPos + 2] * (1 - alpha));
				}


		xGlobalOffset += ch->advance.x / 64; //advance is in Cartesian scale, which means 64 cartesian steps equals 1 pixel
	}


	FT_Done_Face(face);   // Завершение работы с шрифтом face
	FT_Done_FreeType(ft); // Завершение работы FreeType

};

void UserInterfaceItem::RenderButton(Button button, uint8_t* buffer, int posX, int posY, bool isHovered)
{
	//return;
	int fontSizeY = button.height * 10 / 17;
	int fontSizeX = button.width / button.text.size() * 14 / 10;

	int fontSize = std::min(fontSizeX, fontSizeY);

	int borderWidth = button.height / 15;

	glm::u8vec3 blackPixel = glm::u8vec3(0, 0, 0);
	glm::u8vec3 backgroundPixel = glm::u8vec3(200, 200, 200);
	if (isHovered)
		backgroundPixel = glm::u8vec3(255, 255, 150);

	RenderRectangle(buffer, posX, posY, button.width, button.height, blackPixel);
	RenderRectangle(buffer, posX + borderWidth, posY + borderWidth, button.width - borderWidth * 2, button.height - borderWidth * 2, backgroundPixel);

	std::replace(button.text.begin(), button.text.end(), '_', ' ');
	std::transform(button.text.begin(), ++button.text.begin(), button.text.begin(), ::toupper);

	RenderUItext(button.text, fontSize, buffer, posX + button.width / 15, posY + button.height * 10 / 27, blackPixel);
}


 

