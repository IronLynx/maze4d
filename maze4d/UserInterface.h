#pragma once

#include <Utils.h>
#include <Config.h>
#include <sstream>
#include <Player.h>

#include <iomanip>
#include <sstream>
#include <ft2build.h>
#include <list>
#include FT_FREETYPE_H

//#define UI_ACTION_TYPE unsigned int;
//#define UI_SHOW_TYPE unsigned int;

typedef unsigned int UI_ACTION_CODE;

#define UI_ACTION_EXIT 0
#define UI_ACTION_GO 1
#define UI_ACTION_RESTART 2
#define UI_ACTION_NEWGAME 3
#define UI_ACTION_HOTKEYS 4
#define UI_ACTION_MAINMENU 5
#define UI_ACTION_NOTHING 6

#define UI_SHOW_MAINMENU 0
#define UI_SHOW_HOTKEYS 1

class UserInterface
{
public:
	

	UserInterface(int viewWidth, int viewHeight)
	{		
		this->maxWidth = viewWidth;
		this->maxHeight = viewHeight;
		buttonHeight = maxHeight / 10;
		buttonWidth = buttonHeight * 5;
		InitMenu();
	}



	void RenderPlayerinfo(Player* player, uint8_t* buffer)
	{
		int fontSize = maxHeight / 30;
		int lineSpace = fontSize;
		int paddingY = maxHeight / 30;
		int paddingX = maxWidth / 40;
		std::string anglesText = "";		
		std::stringstream stream;


		/*Render first line: XY and XZ*/
		std::string filler = "";
		if (player->angleXY >= 0) filler += " ";
		if (std::abs(player->angleXY) < 10) filler += " ";
		if (std::abs(player->angleXY) < 100) filler += " ";
		stream << "mouseY : " << filler << std::fixed << std::setprecision(2) << player->angleXY;

		filler = "";
		if (player->angleXZ >= 0) filler += " ";
		if (std::abs(player->angleXZ) < 10) filler += " ";
		if (std::abs(player->angleXZ) < 100) filler += " ";
		stream << " mouseX : " << filler << std::fixed << std::setprecision(2) << player->angleXZ;

		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight-paddingY);
		paddingY += lineSpace;


		/*Render second line: XW and ZW*/
		stream.str(std::string()); //empty string

		filler = "";
		if (player->angleXW >= 0) filler += " ";
		if (std::abs(player->angleXW) < 10) filler += " ";
		if (std::abs(player->angleXW) < 100) filler += " ";
		stream << "sMouseY: " << filler << std::fixed << std::setprecision(2) << player->angleXW;

		filler = "";
		if (player->angleZW >= 0) filler += " ";
		if (std::abs(player->angleZW) < 10) filler += " ";
		if (std::abs(player->angleZW) < 100) filler += " ";
		stream << " sMouseX: " << filler << std::fixed << std::setprecision(2) << player->angleZW;

		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer,  paddingX, maxHeight - paddingY);
		paddingY += lineSpace;


		/*Render third line: XW and ZW*/
		stream.str(std::string()); //empty string
		stream << "x: " << std::fixed << std::setprecision(2) << player->lastPos.x;
		stream << " y: " << std::fixed << std::setprecision(2) << player->lastPos.y;
		stream << " z: " << std::fixed << std::setprecision(2) << player->lastPos.z;
		stream << " w: " << std::fixed << std::setprecision(2) << player->lastPos.w;

		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY -= lineSpace;
		
	}

	//renders given text inside buffer with MaxWidth/MaxHeight parameters
	//starting from posX, poxY - from top left corner distance
	void RenderUItext(std::string text, int fontSize, uint8_t* buffer, int posX = 20, int posY = 30, glm::u8vec3 color = glm::u8vec3(255, 255, 255))
	{		
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

			for (int rows = 0; rows < (int) ch->bitmap.rows; rows++)
				for (int xi = 0; xi < (int) ch->bitmap.width; xi++)
					if (rows + yOffset < (int) maxHeight &&
						rows + yOffset > 0 &&
						xi + xGlobalOffset < (int) maxWidth &&
						xi + xGlobalOffset > 0)
					{
						int glyphPixGray = ch->bitmap.buffer[((ch->bitmap.rows - rows - 1) * ch->bitmap.pitch + xi)];
						int pixPos = (rows + yOffset) * maxWidth * 3 + (xi + xGlobalOffset) * 3;

						float alpha = (float)glyphPixGray / (float)ch->bitmap.num_grays;

						buffer[pixPos] = (unsigned int) round(color.x * alpha + buffer[pixPos] * (1 - alpha));
						buffer[pixPos + 1] = (unsigned int) round(color.y * alpha + buffer[pixPos + 1] * (1 - alpha));
						buffer[pixPos + 2] = (unsigned int) round(color.z * alpha + buffer[pixPos + 2] * (1 - alpha));
					}


			xGlobalOffset += ch->advance.x / 64; //advance is in Cartesian scale, which means 64 cartesian steps equals 1 pixel
			;
		}
		
		
		FT_Done_Face(face);   // Завершение работы с шрифтом face
		FT_Done_FreeType(ft); // Завершение работы FreeType
	
	}
	

	//renders given text inside buffer with MaxWidth/MaxHeight parameters
	void RenderRectangle(uint8_t* buffer,int posX, int posY,int width, int height, glm::u8vec3 color)
	{
		int xGlobalOffset = posX;
		int yGlobalOffset = posY;
		int maxPixPos = 0;
		for (int rows = 0; rows < height; rows++)
			for (int xi = 0; xi < width; xi++)
				if (rows + yGlobalOffset < maxHeight && 					
					rows + yGlobalOffset >= 0 && 
					xi + xGlobalOffset < maxWidth &&
					xi + xGlobalOffset >= 0)
				{					
					int pixPos = (rows + yGlobalOffset) * maxWidth * 3 + (xi + xGlobalOffset) * 3;
					if (pixPos > maxPixPos) maxPixPos = pixPos;
					float alpha = 1.0f; //no opacity

					buffer[pixPos] = color.x;// *alpha + buffer[pixPos] * (1 - alpha);
					buffer[pixPos + 1] = color.y;// *alpha + buffer[pixPos + 1] * (1 - alpha);
					buffer[pixPos + 2] = color.z;// *alpha + buffer[pixPos + 2] * (1 - alpha);
				}
		 int maxPosShouldBe = maxWidth*maxHeight * 3 - 1;
		 maxPixPos = maxPixPos;
	}

	void RenderButton(std::string buttonText, uint8_t* buffer,int posX, int posY, bool isHovered = false)
	{
		int fontSize = buttonHeight *10/17;
		int borderWidth = buttonHeight / 15;
		glm::u8vec3 blackPixel = glm::u8vec3(0, 0, 0);
		glm::u8vec3 backgroundPixel = glm::u8vec3(200, 200, 200);
		if (isHovered)
			backgroundPixel = glm::u8vec3(255, 255, 150);

		RenderRectangle(buffer, posX, posY, buttonWidth, buttonHeight, blackPixel);		
		RenderRectangle(buffer, posX + borderWidth, posY + borderWidth, buttonWidth - borderWidth * 2, buttonHeight - borderWidth * 2, backgroundPixel);

		RenderUItext(buttonText, fontSize, buffer, posX + buttonWidth/15, posY+ buttonHeight * 10 /27, blackPixel);
	}
	
	void InitMenu()
	{
		int i = 0;
		buttonList = std::list<Button>();

		buttonList.push_back(Button({ i++, UI_ACTION_GO, "Continue" }));
		buttonList.push_back(Button({ i++, UI_ACTION_RESTART, "Restart" }));
		buttonList.push_back(Button({ i++, UI_ACTION_NEWGAME,"New game" }));
		buttonList.push_back(Button({ i++, UI_ACTION_HOTKEYS,"Hotkeys" }));
		buttonList.push_back(Button({ i++, UI_ACTION_EXIT, "Quit" }));
		activeButton = buttonList.begin();
		activeAction = activeButton->actionCode;
	}

	void Render(uint8_t* buffer)
	{
		switch (showPopUp)
		{
		case UI_SHOW_MAINMENU:
			RenderMainMenu(buffer);
			break;
		case UI_SHOW_HOTKEYS:
			RenderHotKeys(buffer);
			break;
		}
	}

	void RenderMainMenu(uint8_t* buffer)
	{
		int xPosition = (maxWidth - buttonWidth) / 2;
		int yPosition = maxHeight * 9/10;
		int yDelta = 10;		

		yDelta = buttonHeight / 5;
		yPosition = maxHeight - buttonHeight * 3;

		std::list<Button>::iterator it;
		for (it = buttonList.begin(); it != buttonList.end(); ++it) 
		{
			RenderButton(it->text, buffer, xPosition, yPosition - yDelta, &*it == &*activeButton);
			yPosition = yPosition - buttonHeight - yDelta;
		}		
	}

	void RenderHotKeys(uint8_t* buffer)
	{
		int width = maxWidth*9/10;
		int height = maxHeight*9/10;
		int xPosition = (maxWidth - width)/2;
		int yPosition = maxHeight - height - height/20;
		

		glm::u8vec3 blackPixel = glm::u8vec3(0, 0, 0);
		glm::u8vec3 backgroundPixel = glm::u8vec3(200, 200, 200);

		RenderRectangle(buffer, xPosition, yPosition, width, height, blackPixel);
		RenderRectangle(buffer, xPosition + 3, yPosition + 3, width - 6, height - 6, backgroundPixel);

		std::list<std::string> lines = std::list<std::string>();
		lines.push_back("First person four - dimensional maze game");
		lines.push_back("visualized by 3D cross - sections.");
		lines.push_back(" ");
		lines.push_back("================== Controls ==================");
		lines.push_back("    ");
		lines.push_back("------------Movement------------");
		lines.push_back("Forward (+x) : W, Back (-x) : S");
		lines.push_back("Up      (+y) : R, Down (-y) : F");
		lines.push_back("Right   (+z) : D, Left (-z) : A");
		lines.push_back("Ana     (+w) : T, Kata (-w) : G");
		lines.push_back(" ");
		lines.push_back("------------Rotation------------");
		lines.push_back("XY (pitch)   : I, K (or mougseY)");
		lines.push_back("XZ (yaw)     : L, J (or mouseX)");
		lines.push_back("XW           : O, U (or mouseY + LSHIFT)");
		lines.push_back("YZ (roll)    : E, Q");
		lines.push_back("YW           : Z, C");
		lines.push_back("ZW           : M, N (or mouseX + LSHIFT)");
		lines.push_back(" ");
		lines.push_back("------------Other------------");
		lines.push_back("Reset player   : P");
		lines.push_back("Reset rotation : B");
		lines.push_back("Align W-angles : TAB");
		lines.push_back("Noclip         : F8");
		lines.push_back("Fullscreen     : F11");
		lines.push_back("Lock mouse     : SPACE or mouse button");
		lines.push_back("Menu           : ESC");

		
		int fontSizeY = height * 9/10 / lines.size();

		int maxLength = 0;
		for (std::list<std::string>::iterator lit = lines.begin(); lit != lines.end(); ++lit)
			maxLength = std::max(maxLength, (int)lit->length());
		int fontSizeX = width* 95/100 / maxLength * 15/10;

		int fontSize = std::min(fontSizeY, fontSizeX);
		int yDelta = fontSize ;

		std::list<std::string>::iterator it;
		for (it = lines.begin(); it != lines.end(); ++it)
		{
			RenderUItext(*it, fontSize, buffer, xPosition + width*1/10, yPosition + height*95/100, blackPixel);
			yPosition = yPosition - yDelta;
		}		
	}


	//===================================
	//Menu   Controller
	void NextMenuItem()
	{
		if (showPopUp == UI_SHOW_MAINMENU)
		{
			activeButton++;
			if (activeButton == buttonList.end()) activeButton = buttonList.begin();
			activeAction = activeButton->actionCode;
		}
	}

	void PreviousMenuItem()
	{		
		if (showPopUp == UI_SHOW_MAINMENU)
		{
			if (activeButton == buttonList.begin()) activeButton = buttonList.end();
			activeButton--;
			activeAction = activeButton->actionCode;
		}
	}

	UI_ACTION_CODE OnCancel()
	{
		if (showPopUp == UI_SHOW_MAINMENU)
			return UI_ACTION_GO;
		else
		{
			activeAction = activeButton->actionCode;
			showPopUp = UI_SHOW_MAINMENU;
			reRenderBackground = true;
			return UI_ACTION_NOTHING;
		}		
	}

	UI_ACTION_CODE OnSelect()
	{
		switch (activeAction)
		{
		case UI_ACTION_EXIT:
			return UI_ACTION_EXIT;
			//glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case UI_ACTION_GO:
			return UI_ACTION_GO;
			break;
		case UI_ACTION_NEWGAME:
			return UI_ACTION_NEWGAME;
			break;
		case UI_ACTION_RESTART:
			return UI_ACTION_RESTART;
			break;		
		case UI_ACTION_HOTKEYS:
			reRenderBackground = true;
			showPopUp = UI_SHOW_HOTKEYS;
			activeAction = UI_ACTION_MAINMENU;
			return UI_ACTION_NOTHING;
			break;
		case UI_ACTION_MAINMENU:
			return OnCancel();
			break;
		}
		return UI_ACTION_NOTHING;
	}
	//===================================


	struct Button 
	{
		int id;
		unsigned int actionCode;
		std::string text;
	};

	std::list<Button> buttonList;
	std::list<Button>::iterator activeButton;
	unsigned int activeAction;

	int buttonWidth = 200;
	int buttonHeight = 50;

	unsigned int showPopUp = 0;
	bool reRenderBackground = true;

	int maxWidth;
	int maxHeight;
};