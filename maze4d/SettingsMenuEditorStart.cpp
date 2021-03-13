#include <UserInterfaceClasses.h>

SettingsMenuEditorStart::SettingsMenuEditorStart(Game* game) : SettingsMenu()
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;
	this->game = game;

	this->category = game->cfg->GetParamByTag("editor_room_size").category;

	ReloadParams();

	backButton = Button(paramList.begin()->button);
	backButton.actionCode = UI_ACTION_CLOSE_ITEM;
	backButton.text = "Start";
	backButton.helpText = "Generate new emptycube box with enabled editor tools";

	activeParam = paramList.end();
	activeAction = UI_ACTION_CLOSE_ITEM;
}

UI_ACTION_CODE SettingsMenuEditorStart::OnCancel()
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

UI_ACTION_CODE SettingsMenuEditorStart::OnSelect()
{
	//iterator.end() means "Start" button is currently active
	if (activeParam == paramList.end())
	{
		game->editorToolsEnabled = true;
		game->NewEditor();
		return backButton.actionCode;
	}

	else
	{
		SwitchEditMode();
		return UI_ACTION_NOTHING;
	}
}