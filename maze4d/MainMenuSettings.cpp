#include <UserInterfaceClasses.h>

MainMenuSettings::MainMenuSettings(Game* game)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;
	this->game = game;

	int i = 0;
	buttonList = std::list<Button>();

	Config::CFG_CATEGORY_LIST categories = game->cfg->GetCategoriesList();

	for (Config::CFG_CATEGORY_LIST::iterator catIt = categories.begin(); catIt!=categories.end(); catIt++)
		buttonList.push_back(Button({ i++, UI_ACTION_OPEN_NEW_ITEM, *catIt }));

	buttonList.push_back(Button({ i++, UI_ACTION_CLOSE_ITEM, "Back" }));

	RecalculateButtonWidth();

	activeButton = buttonList.begin();
	activeAction = activeButton->actionCode;
}

UI_ACTION_CODE MainMenuSettings::OnSelect()
{
	UI_ACTION_CODE action = activeButton->actionCode;

	if (activeButton->actionCode == UI_ACTION_CLOSE_ITEM)
		action = UI_ACTION_CLOSE_ITEM;

	if (activeButton->actionCode == UI_ACTION_OPEN_NEW_ITEM)
		newWindow = new SettingsMenu(game, activeButton->text);

	return action;
}