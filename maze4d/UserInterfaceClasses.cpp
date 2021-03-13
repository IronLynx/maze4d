#include <UserInterfaceClasses.h>

Shader* UserInterfaceItem::shader = nullptr;
GameGraphics* UserInterfaceItem::graphics = nullptr;

void UserInterfaceItem::InitGL()
{
	if (shader == nullptr)
	{
		shader = new Shader();
		shader->LoadFromFiles("VertexShader.hlsl", "FragmentShader.hlsl");
	}

	if (graphics == nullptr)
	{
		graphics = new GameGraphics(shader, 0.0f, 0.0f, 1.0f, 1.0f);
	}
}

void UserInterfaceItem::RenderRectangle(int posX, int posY, int width, int height, glm::u8vec3 color)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	float fPosX = 2.0f * posX / maxWidth - 1.0f;
	float fPosY = 2.0f * posY / maxHeight - 1.0f;
	float fWidth = 2.0f * width / maxWidth;
	float fHeight = 2.0f * height / maxHeight;

	//GameGraphics gr = GameGraphics(shader, fPosX, fPosY, fWidth, fHeight);
	graphics->SetNewPosition(fPosX, fPosY, fWidth, fHeight);

	uint8_t* onePixelBuffer = new uint8_t[4];
	memset(onePixelBuffer, 0, 4);
	onePixelBuffer[0] = color.x;
	onePixelBuffer[1] = color.y;
	onePixelBuffer[2] = color.z;
	onePixelBuffer[3] = 255; 

	graphics->Draw(color);
	delete[] onePixelBuffer;
}

void UserInterfaceItem::RenderUItext(std::string text, int fontSize, int posX, int posY, glm::u8vec3 color)
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

	//GameGraphics gr = GameGraphics(shader, 0.0f, 0.0f, 0.5f, 0.5f);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		char letter = *c;
		if (FT_Load_Char(face, *c, FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

		FT_GlyphSlot ch = face->glyph;

		int yOffset = yGlobalOffset + ch->bitmap_top - ch->bitmap.rows; //calculate up/down y offset for such letters as 'g'

		float fPosX = 2.0f * xGlobalOffset / maxWidth - 1.0f;
		float fPosY = 2.0f * yOffset / maxHeight - 1.0f;
		float fWidth = 2.0f * ch->bitmap.pitch / maxWidth;
		float fHeight = 2.0f * ch->bitmap.rows / maxHeight;


		graphics->SetNewPosition(fPosX, fPosY, fWidth, fHeight);

		int maxWidth = ch->bitmap.pitch;
		int maxHeight = ch->bitmap.rows;
		int bufSize = 4 * maxHeight * maxWidth;

		uint8_t* buffer = new uint8_t[bufSize];

		for (int rows = 0; rows < maxHeight; rows++)
			for (int xi = 0; xi < maxWidth; xi++)
			{
				int glyphPixGray = ch->bitmap.buffer[((ch->bitmap.rows - rows - 1) * ch->bitmap.pitch + xi)];
				int pixPos = rows * maxWidth * 4 + xi * 4;

				float alpha = (float)glyphPixGray / (float)ch->bitmap.num_grays;

				buffer[pixPos] = (uint8_t)round(color.x);
				buffer[pixPos + 1] = (uint8_t)round(color.y);
				buffer[pixPos + 2] = (uint8_t)round(color.z);
				buffer[pixPos + 3] = (uint8_t)round(255 * alpha);
			}

		graphics->Draw(buffer, maxWidth, maxHeight, GL_RGBA);
		delete[] buffer;

		xGlobalOffset += ch->advance.x / 64; //advance is in Cartesian scale, which means 64 cartesian steps equals 1 pixel
	}


	FT_Done_Face(face);   // Завершение работы с шрифтом face
	FT_Done_FreeType(ft); // Завершение работы FreeType
	
};

void UserInterfaceItem::RenderButton(Button button, int posX, int posY, bool isHovered)
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

	RenderRectangle(posX, posY, button.width, button.height, blackPixel);
	RenderRectangle(posX + borderWidth, posY + borderWidth, button.width - borderWidth * 2, button.height - borderWidth * 2, backgroundPixel);
	

	std::replace(button.text.begin(), button.text.end(), '_', ' ');
	std::transform(button.text.begin(), ++button.text.begin(), button.text.begin(), ::toupper);

	RenderUItext(button.text, fontSize, posX + button.width / 15, posY + button.height * 10 / 27, blackPixel);

	//Add helper text at the bottom of the screen
	if (isHovered)
	{
		if (button.helpText.size() == 0)
			button.helpText = " ";
		int helperFontSizeX = game->viewWidth / button.helpText.size() * 14 / 10;
		int helperFontSizeY = game->viewHeight / 35;

		int helperFontSize = std::min(helperFontSizeX, helperFontSizeY);

		RenderRectangle( 0, 0, game->viewWidth, helperFontSize+10, backgroundPixel);
		RenderUItext(button.helpText, helperFontSize, 10, 10, blackPixel);
	}
}


 

