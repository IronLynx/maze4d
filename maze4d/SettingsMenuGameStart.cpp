#include <UserInterfaceClasses.h>

SettingsMenuGameStart::SettingsMenuGameStart(Game* game)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;
	this->game = game;

	this->category = game->cfg->GetParamByTag("maze_size_x").category;

	ReloadParams();

	backButton = Button(paramList.begin()->button);
	backButton.actionCode = UI_ACTION_CLOSE_ITEM;
	backButton.text = "Start";

	activeParam = paramList.end();
	activeAction = UI_ACTION_CLOSE_ITEM;
}

UI_ACTION_CODE SettingsMenuGameStart::OnCancel()
{
	if (isEditMode)
	{
		isEditMode = false;
		inputParameter = 0;
		return UI_ACTION_NOTHING;
	}
	else
	{
		return UI_ACTION_CLOSE_ITEM;
	}
}

UI_ACTION_CODE SettingsMenuGameStart::OnSelect()
{
	//iterator.end() means "Start" button is currently active
	if (activeParam == paramList.end())
	{
		game->NewGame();
		return backButton.actionCode;
	}

	else
	{
		SwitchEditMode();
		return UI_ACTION_NOTHING;
	}
}