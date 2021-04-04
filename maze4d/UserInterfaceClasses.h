#pragma once

#include <shader.h>
#include <Game.h>
#include <Config.h>
#include <Player.h>

//#include <glad/glad.h> // generated from https://glad.dav1d.de

#include <iomanip>
#include <sstream>

#include <ft2build.h>
#include <freetype/ftbitmap.h>
#include <list>
#include <stack> 

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>

#include <iomanip>
#include <sstream>

#include <Utils.h>
#include FT_FREETYPE_H

//#define UI_ACTION_TYPE unsigned int;
//#define UI_SHOW_TYPE unsigned int;

typedef unsigned int UI_ACTION_CODE;

#define UI_ACTION_EXIT 0
#define UI_ACTION_CLOSE_ITEM 1
#define UI_ACTION_RESTART 2
#define UI_ACTION_NEWGAME 3
#define UI_ACTION_HOTKEYS 4
#define UI_ACTION_MAINMENU 5
#define UI_ACTION_NOTHING 6
#define UI_ACTION_SETTINGS 7
#define UI_ACTION_OPEN_NEW_ITEM 8
#define UI_ACTION_OPEN_NEW_CLOSE_OLD 9
#define UI_ACTION_NEWEDITOR 10

static std::string float2str(float value, unsigned int decimals = 2, unsigned int alignLength = 1)
{
	std::string result = "";
	std::stringstream stream;
	
	stream << std::fixed << std::setprecision(decimals) << value;
	result = stream.str();
	while (result.length() < alignLength)
		result = " " + result;

	return result;
};

static std::string vec2str(glm::vec4 vec)
{
	unsigned int floatLength = 4;
	std::stringstream stream;
	//Render vectorx
	stream.str(std::string()); //empty string
	stream << "(" << float2str(vec.x, 2, floatLength);
	stream << "," << float2str(vec.y, 2, floatLength);
	stream << "," << float2str(vec.z, 2, floatLength);
	stream << "," << float2str(vec.w, 2, floatLength);
	stream << ")";
	return stream.str();
}


struct Button
{
	int id;
	unsigned int actionCode;
	std::string text;
	std::string helpText;

	int width;
	int height;
};

class  UserInterfaceItem
{
public:
	UserInterfaceItem() {}

	UserInterfaceItem(Game* game) 
	{ 
		this->game = game; 
		activeAction = UI_ACTION_NOTHING;
	}
	
	virtual void Render(uint8_t* buffer) {}
	virtual UI_ACTION_CODE OnSelect() { return UI_ACTION_NOTHING; }
	virtual UI_ACTION_CODE OnCancel() { return UI_ACTION_CLOSE_ITEM; }
	virtual UI_ACTION_CODE OnKeyInput(unsigned int keyCode) { return UI_ACTION_NOTHING; }
	virtual UserInterfaceItem* NewWindow() { return newWindow; }

	virtual void NextMenuItem() {}
	virtual void PreviousMenuItem() {}

	//void RenderUItext(std::string text, int fontSize, uint8_t* buffer, int posX = 20, int posY = 30, glm::u8vec3 color = glm::u8vec3(255, 255, 255));
	//void RenderRectangle(uint8_t* buffer, int posX, int posY, int width, int height, glm::u8vec3 color);

	void RenderRectangle(int posX, int posY, int width, int height, glm::u8vec3 color);
	void RenderUItext(std::string text, int fontSize, int posX = 20, int posY = 30, glm::u8vec3 color = glm::u8vec3(255, 255, 255));

	void RenderButton(Button button, int posX, int posY, bool isHovered = false);
	int FPS = 0;

	static void InitGL();
	static void ClearShaders()
	{
		if (shader != nullptr)
			delete shader;
		shader = nullptr;
		if (graphics != nullptr)
			delete graphics;
		graphics = nullptr;
	}

protected:
	UserInterfaceItem* newWindow = nullptr;
	UI_ACTION_CODE activeAction;
	Game* game = nullptr;
	static Shader* shader;
	static RectangleGraphics* graphics;
};

class HotKeysMenu : public UserInterfaceItem
{
public:
	HotKeysMenu(Game* game) : UserInterfaceItem(game) {}

	virtual void Render(uint8_t* buffer);
	virtual UI_ACTION_CODE OnSelect();
	virtual UI_ACTION_CODE OnKeyInput(unsigned int keyCode);
};

class SettingsMenu : public UserInterfaceItem
{
public:
	SettingsMenu() : UserInterfaceItem() {}
	SettingsMenu(Game* game, std::string category = "");

	virtual void Render(uint8_t* buffer);

	virtual UI_ACTION_CODE OnSelect();
	virtual UI_ACTION_CODE OnCancel();
	virtual UI_ACTION_CODE OnKeyInput(unsigned int keyCode);
	virtual void NextMenuItem();
	virtual void PreviousMenuItem();

protected:
	Button backButton;

	virtual void OnClose();

	struct Parameter
	{
		std::string paramTag;
		Config::paramValues Value;
		Button button;
	};

	std::string category;

	std::list<Parameter> paramList;
	std::list<Parameter>::iterator activeParam;
	UI_ACTION_CODE activeAction;

	bool isEditMode = false;
	float inputParameter; //keyboard inputed but not yet saved parameter
	std::string inputParameterTxt;

	std::string ParamTextForDisplay(Parameter param);
	std::string ParamTextForDisplay(float value, CFG_VALUE_TYPE valueType);

	void ReloadParams();
	void SwitchEditMode();
	void RecalculateButtonWidth();
};

class SettingsMenuGameStart : public SettingsMenu
{
public:
	SettingsMenuGameStart(Game* game);
	virtual UI_ACTION_CODE OnCancel();
	virtual UI_ACTION_CODE OnSelect();
};

class SettingsMenuEditorStart : public SettingsMenu
{
public:
	SettingsMenuEditorStart(Game* game);
	virtual UI_ACTION_CODE OnCancel();
	virtual UI_ACTION_CODE OnSelect();
};


class MainMenu : public UserInterfaceItem
{
public:
	MainMenu() : UserInterfaceItem() {}
	MainMenu(Game* game) ;

	void RecalculateButtonWidth();

	virtual void Render(uint8_t* buffer);
	virtual void NextMenuItem();
	virtual void PreviousMenuItem();
	virtual UI_ACTION_CODE OnSelect();

protected:
	std::list<Button> buttonList;
	std::list<Button>::iterator activeButton;
	UI_ACTION_CODE activeAction;
};

class MainMenuSettings : public MainMenu
{
public:
	MainMenuSettings(Game* game);
	virtual UI_ACTION_CODE OnSelect();
};

class MainUiController : public UserInterfaceItem
{
public:
	MainUiController(Game* game);

	virtual void Render(uint8_t* buffer);
	virtual void NextMenuItem(); 
	virtual void PreviousMenuItem();
	virtual UI_ACTION_CODE OnSelect();
	virtual UI_ACTION_CODE OnCancel();
	virtual UI_ACTION_CODE OnKeyInput(unsigned int keyCode);

	void CloseCurrentItem();
	
	bool reRenderBackground = true;
	bool isMenu = true;
	void RenderPlayerinfo(Player* player, uint8_t* buffer);
private:	
	Game* game;
	std::stack<UserInterfaceItem*> openedUiList;
	void ExitMenu() { isMenu = false; }
	
};





