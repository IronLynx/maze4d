#include <UserInterfaceClasses.h>

void HotKeysMenu::Render(uint8_t* buffer)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	int width = maxWidth * 9 / 10;
	int height = maxHeight * 9 / 10;
	int xPosition = (maxWidth - width) / 2;
	int yPosition = maxHeight - height - height / 20;


	glm::u8vec3 blackPixel = glm::u8vec3(0, 0, 0);
	glm::u8vec3 backgroundPixel = glm::u8vec3(200, 200, 200);

	RenderRectangle(xPosition, yPosition, width, height, blackPixel);
	RenderRectangle(xPosition + 3, yPosition + 3, width - 6, height - 6, backgroundPixel);

	std::list<std::string> lines = std::list<std::string>();
	lines.push_back("First person four - dimensional maze game");
	lines.push_back("visualized by 3D cross - sections.");
	lines.push_back(" ");
	lines.push_back("================== Controls ==================");
	lines.push_back("    ");
	lines.push_back("------------Movement------------");
	lines.push_back("Forward (+x)  : W, Back (-x) : S");
	lines.push_back("Up      (+y)  : R, Down (-y) : F");
	lines.push_back("Right   (+z)  : D, Left (-z) : A");
	lines.push_back("Ana     (+w)  : T, Kata (-w) : G");
	lines.push_back(" ");
	lines.push_back("------------Rotation------------");
	lines.push_back("XZ (yaw)      : L, J (or mouseX)");
	lines.push_back("XY (pitch)    : I, K (or mougseY)");	
	lines.push_back("YZ (roll)     : E, Q");
	lines.push_back("ZW            : M, N (or mouseX + LSHIFT)");
	lines.push_back("XW            : O, U (or mouseY + LSHIFT)");
	lines.push_back("YW            : Z, C");	
	lines.push_back("Align W-angle : TAB");
	lines.push_back("Reset W-angle : B");
	lines.push_back(" ");
	lines.push_back("------------Editor tools------------");
	lines.push_back("New cube      : Left Mouse Click");
	lines.push_back("Delete cube   : Right Mouse Click");
	lines.push_back("Solid cube    : 1");
	lines.push_back("Light cube    : 2");
	lines.push_back("Clear all     : Delete");
	lines.push_back("Decrease transparency  : Scroll Down");
	lines.push_back("Increase transparency  : Scroll Up");
	lines.push_back(" ");
	lines.push_back("------------Other------------");
	lines.push_back("Reset player  : P");
	lines.push_back("Noclip        : F8");
	lines.push_back("Fullscreen    : F11");
	lines.push_back("Lock mouse    : SPACE");
	lines.push_back("Menu          : ESC");

	int fontSizeY = height * 9 / 10 / lines.size();

	int maxLength = 0;
	for (std::list<std::string>::iterator lit = lines.begin(); lit != lines.end(); ++lit)
		maxLength = std::max(maxLength, (int)lit->length());
	int fontSizeX = width * 95 / 100 / maxLength * 15 / 10;

	int fontSize = std::min(fontSizeY, fontSizeX);
	int yDelta = fontSize;

	std::list<std::string>::iterator it;
	for (it = lines.begin(); it != lines.end(); ++it)
	{
		RenderUItext(*it, fontSize,  xPosition + width * 1 / 10, yPosition + height * 95 / 100, blackPixel);
		yPosition = yPosition - yDelta;
	}
}

UI_ACTION_CODE HotKeysMenu::OnSelect() { return UI_ACTION_CLOSE_ITEM; }
UI_ACTION_CODE HotKeysMenu::OnKeyInput(unsigned int keyCode) { return UI_ACTION_CLOSE_ITEM; }