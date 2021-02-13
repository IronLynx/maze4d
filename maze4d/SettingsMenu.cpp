#include <UserInterfaceClasses.h>

SettingsMenu::SettingsMenu(Game* game, std::string category) : UserInterfaceItem(game)
{
	this->category = category;
	ReloadParams();
	activeParam = paramList.begin();
	activeAction = activeParam->button.actionCode;

	backButton = Button(paramList.begin()->button);
	backButton.actionCode = UI_ACTION_CLOSE_ITEM;
	backButton.text = "Back";
	backButton.helpText = "";
}

void SettingsMenu::Render(uint8_t* buffer)
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	RecalculateButtonWidth();

	std::list<Parameter>::iterator it = paramList.begin();
	int xPosition = (maxWidth - it->button.width - it->button.width * 10 / 25) / 2;
	int yDelta = it->button.height / 5;
	int yPosition = maxHeight - it->button.height * 2;

	for (it = paramList.begin(); it != paramList.end(); ++it)
	{

		Button valueDisplayButton = Button(it->button);
		valueDisplayButton.width = valueDisplayButton.height * 7 * 10 / 25;

		valueDisplayButton.text = ParamTextForDisplay(*it);
		while (valueDisplayButton.text.size() <= 6)
			valueDisplayButton.text = " " + valueDisplayButton.text;

		bool paramIsEditing = (isEditMode && it == activeParam);
		if (paramIsEditing)
			valueDisplayButton.text = inputParameterTxt;// ParamTextForDisplay(inputParameter, it->Value.valueType);

		RenderButton(it->button, buffer, xPosition, yPosition - yDelta, it == activeParam);
		RenderButton(valueDisplayButton, buffer, xPosition + it->button.width * 105 / 100, yPosition - yDelta, isEditMode && it == activeParam);
		yPosition = yPosition - it->button.height - yDelta;
	}

	RenderButton(backButton, buffer, xPosition, yPosition - yDelta, activeParam == paramList.end());
	yPosition = yPosition - backButton.height - yDelta;
}

void SettingsMenu::SwitchEditMode()
{
	if (!isEditMode)
	{
		isEditMode = true;
		Parameter par = *activeParam;
		inputParameterTxt = ParamTextForDisplay(par);
		inputParameter = std::stof(par.Value.value);
	}
	else
	{
		isEditMode = false;
		game->cfg->SetParameter(activeParam->paramTag, inputParameter);
		game->cfg->WriteFile();
		ReloadParams();
	}
}

void SettingsMenu::OnClose()
{
	game->ApplyNewParameters();
}

UI_ACTION_CODE SettingsMenu::OnSelect()
{
	if (activeParam == paramList.end())
	{
		OnClose();
		return backButton.actionCode;
	}

	SwitchEditMode();

	return UI_ACTION_NOTHING;
}

UI_ACTION_CODE SettingsMenu::OnCancel()
{
	if (isEditMode)
	{
		isEditMode = false;
		inputParameter = 0;
		return UI_ACTION_NOTHING;
	}
	
	OnClose();
	return UI_ACTION_CLOSE_ITEM;
}

UI_ACTION_CODE SettingsMenu::OnKeyInput(unsigned int keyCode)
{
	if (isEditMode)
	{
		isEditMode = true;
		float inputParameterBefore = inputParameter;

		std::stringstream stream;
		stream << std::fixed;
		if (activeParam->Value.valueType == CFG_TYPE_FLOAT)
			stream << std::setprecision(2);
		else
			stream << std::setprecision(0);
		stream << inputParameterBefore;
		std::string CurValue = inputParameterTxt;

		if (keyCode >= GLFW_KEY_0 && keyCode <= GLFW_KEY_9)
		{
			if (inputParameterTxt == ParamTextForDisplay(*activeParam))
				inputParameterTxt = "";

			if (inputParameterTxt == "0")
				inputParameterTxt = keyCode;

			if (inputParameterTxt.size() < 6)
				inputParameterTxt += keyCode;
		}


		if (keyCode == GLFW_KEY_BACKSPACE)
			if (inputParameterTxt.size() > 1)
				inputParameterTxt = inputParameterTxt.erase(CurValue.size() - 1);
			else
				inputParameterTxt = "0";

		if (keyCode == GLFW_KEY_PERIOD || keyCode == GLFW_KEY_COMMA)
			if (activeParam->Value.valueType == CFG_TYPE_FLOAT)
			{
				if (inputParameterTxt == ParamTextForDisplay(*activeParam))
					inputParameterTxt = "";

				if (inputParameterTxt == "0")
					inputParameterTxt = keyCode;

				int pos = inputParameterTxt.find('.');
				if (pos < 0)
					inputParameterTxt += ".";
			}

		try
		{
			inputParameter = std::stof(inputParameterTxt);
			inputParameter = round(inputParameter * 100) / 100;
		}
		catch (...)
		{
			inputParameter = inputParameterBefore;
		}
	}
	return UI_ACTION_NOTHING;
}

void SettingsMenu::NextMenuItem()
{
	if (isEditMode)
	{
		if (activeParam->Value.valueType == CFG_TYPE_BOOL)
			inputParameter = abs(round(inputParameter - 1));
		else
			inputParameter--;

		inputParameterTxt = ParamTextForDisplay(inputParameter, activeParam->Value.valueType);
		return;
	}
	else
		if (activeParam == paramList.end())
			activeParam = paramList.begin();
		else
			activeParam++;
}

void SettingsMenu::PreviousMenuItem()
{
	if (isEditMode)
	{
		if (activeParam->Value.valueType == CFG_TYPE_BOOL)
			inputParameter = abs(round(inputParameter - 1));
		else
			inputParameter++;
		inputParameterTxt = ParamTextForDisplay(inputParameter, activeParam->Value.valueType);
		return;
	}
	else
		if (activeParam == paramList.begin())
			activeParam = paramList.end();
		else
			activeParam--;
}


//PRIVATE::

std::string SettingsMenu::ParamTextForDisplay(Parameter param)
{
	float value = game->cfg->GetFloat(param.paramTag);
	return ParamTextForDisplay(value, param.Value.valueType);
}

std::string SettingsMenu::ParamTextForDisplay(float value, CFG_VALUE_TYPE valueType)
{
	std::stringstream stream;

	if (valueType == CFG_TYPE_BOOL)
		stream << (value ? "True" : "False");
	else if (valueType == CFG_TYPE_INT)
		stream << std::fixed << std::setprecision(0) << value;
	else if (valueType == CFG_TYPE_FLOAT)
		stream << std::fixed << std::setprecision(1) << value;

	std::string valueText = stream.str();

	return valueText;
}

void SettingsMenu::ReloadParams()
{
	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	std::string activeTag = "";
	if (paramList.size() > 0)
		activeTag = activeParam->paramTag;


	int i = 0;
	paramList = std::list<Parameter>();
	Config::CFG_PARAM_LIST cfgList = game->cfg->GetParamList();
	Config::CFG_PARAM_LIST::iterator parIt = cfgList.begin();
	for (parIt = cfgList.begin(); parIt != cfgList.end(); ++parIt)
	{
		std::string tag = parIt->tag;
		Config::paramValues cfgParam = game->cfg->GetParamByTag(tag);

		if (cfgParam.category == category || category == "")
		{
			std::string buttonText = tag;
			std::replace(buttonText.begin(), buttonText.end(), '_', ' ');
			std::transform(buttonText.begin(), ++buttonText.begin(), buttonText.begin(), ::toupper);

			Parameter par = Parameter({ tag, cfgParam, Button({ i++, UI_ACTION_NOTHING, buttonText, cfgParam.comment }) });
			paramList.push_back(par);
		}
	}
	std::list<Parameter>::iterator it;
	for (it = paramList.begin(); it != paramList.end(); ++it)
	{
		if (activeTag == it->paramTag)
		{
			activeParam = it;
			activeAction = activeParam->button.actionCode;
		}
	}

	RecalculateButtonWidth();
}

void SettingsMenu::RecalculateButtonWidth()
{
	int elementCountForCalc = paramList.size();
	if (elementCountForCalc < 5) elementCountForCalc = 5;

	int newHeight = game->viewHeight / (elementCountForCalc + 6);
	int newWidth = newHeight * 7;

	if (newWidth * 105 / 100 + newWidth * 10 / 25 >= game->viewWidth / 2)
		newWidth = game->viewWidth * 10 / 25;

	std::list<Parameter>::iterator it;
	for (it = paramList.begin(); it != paramList.end(); ++it)
	{
		it->button.height = newHeight;
		it->button.width = newWidth;
	}

	backButton.width = newWidth;
	backButton.height = newHeight;
}