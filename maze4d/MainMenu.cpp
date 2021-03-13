#include <UserInterfaceClasses.h>

MainMenu::MainMenu(Game* game) : UserInterfaceItem(game)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	int i = 0;
	buttonList = std::list<Button>();

	buttonList.push_back(Button({ i++, UI_ACTION_CLOSE_ITEM, "Continue", "Back to the game" }));
	//buttonList.push_back(Button({ i++, UI_ACTION_RESTART, "Restart", "Restart application and reload all" }));
	buttonList.push_back(Button({ i++, UI_ACTION_NEWGAME,"New game", "Generate new maze and start new game" }));
	buttonList.push_back(Button({ i++, UI_ACTION_NEWEDITOR, "New editor", "Create empty box with enabled free for all editor tools" }));
	buttonList.push_back(Button({ i++, UI_ACTION_HOTKEYS,"Hotkeys", "List of hotkeys that can be used during the game" }));
	buttonList.push_back(Button({ i++, UI_ACTION_SETTINGS,"Settings", "Adjustable parameters" }));
	buttonList.push_back(Button({ i++, UI_ACTION_EXIT, "Quit", "Close application and exist the game" }));

	RecalculateButtonWidth();

	activeButton = buttonList.begin();
	activeAction = activeButton->actionCode;
}

void MainMenu::RecalculateButtonWidth()
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	std::list<Button>::iterator it;
	for (it = buttonList.begin(); it != buttonList.end(); ++it)
	{
		it->height = maxHeight / (buttonList.size() + 5);
		it->width = it->height * 5;
	}
}

void MainMenu::Render(uint8_t* buffer)
{
	RecalculateButtonWidth(); //in case if resolution has changed

	std::list<Button>::iterator it = buttonList.begin();
	int xPosition = (game->viewWidth - it->width) / 2;
	int yDelta = it->height / 5;
	int yPosition = game->viewHeight - it->height * 3;

	for (it = buttonList.begin(); it != buttonList.end(); ++it)
	{
		//it->Render(buffer, xPosition, yPosition - yDelta, buttonWidth,buttonHeight, &*it == &*activeButton);
		RenderButton(*it, xPosition, yPosition - yDelta, &*it == &*activeButton);
		yPosition = yPosition - it->height - yDelta;
	}
	
}

UI_ACTION_CODE MainMenu::OnSelect()
{
	switch (activeButton->actionCode)
	{
	case UI_ACTION_RESTART:
		game->player.Reset();
		game->NeedReconfigureResolution = true; //for restart
		return UI_ACTION_RESTART;
		break;
	case UI_ACTION_NEWGAME:
		newWindow = new SettingsMenuGameStart(game);
		return UI_ACTION_OPEN_NEW_CLOSE_OLD;
		break;
	case UI_ACTION_NEWEDITOR:
		newWindow = new SettingsMenuEditorStart(game);
		return UI_ACTION_OPEN_NEW_CLOSE_OLD;
		break;
	case UI_ACTION_HOTKEYS:
		newWindow = new HotKeysMenu(game);
		return UI_ACTION_OPEN_NEW_ITEM;
		break;
	case UI_ACTION_SETTINGS:
		newWindow = new MainMenuSettings(game);
		return UI_ACTION_OPEN_NEW_ITEM;
		break;
	}

	return activeButton->actionCode;
}

void MainMenu::NextMenuItem()
{
	activeButton++;
	if (activeButton == buttonList.end()) activeButton = buttonList.begin();
	activeAction = activeButton->actionCode;
}

void MainMenu::PreviousMenuItem()
{
	if (activeButton == buttonList.begin()) activeButton = buttonList.end();
	activeButton--;
	activeAction = activeButton->actionCode;
}