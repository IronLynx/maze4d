#include <UserInterfaceClasses.h>
#include <math.h>

MainUiController::MainUiController(Game* game) : UserInterfaceItem(game)
{
	openedUiList = std::stack<UserInterfaceItem*>();
	activeAction = UI_ACTION_MAINMENU;
	this->game = game;
	openedUiList.push(new MainMenu(game));
}

void MainUiController::Render(uint8_t* buffer)
{
	if (openedUiList.empty()) isMenu = false;

	if (openedUiList.empty() || reRenderBackground)
	{
		game->Render(buffer);
		reRenderBackground = false;
	}

	if (!openedUiList.empty())
		openedUiList.top()->Render(buffer);
	else
	{
		if (game->cfg->GetBool("display_coords"))
			this->RenderPlayerinfo(&game->player, buffer);
	}
}

void MainUiController::NextMenuItem() 
{ 
	if (!openedUiList.empty()) 
		openedUiList.top()->NextMenuItem(); 
}

void MainUiController::PreviousMenuItem()
 { 
	 if (!openedUiList.empty()) 
		 openedUiList.top()->PreviousMenuItem(); 
 }

void MainUiController::CloseCurrentItem()
{
	UserInterfaceItem* ptr = openedUiList.top();
	openedUiList.pop();
	delete ptr;
	if (openedUiList.empty())
		ExitMenu();
	reRenderBackground = true;
}

UI_ACTION_CODE MainUiController::OnSelect()
{
	if (openedUiList.empty())
		return UI_ACTION_NOTHING;
	else
	{
		UI_ACTION_CODE action = openedUiList.top()->OnSelect();

		if (action == UI_ACTION_OPEN_NEW_ITEM)
		{
			UserInterfaceItem* ptr = openedUiList.top()->NewWindow();
			if (ptr != nullptr)
				openedUiList.push(ptr);
			reRenderBackground = true;
		}

		if (action == UI_ACTION_CLOSE_ITEM ||
			action == UI_ACTION_EXIT)
			CloseCurrentItem();

		if (action == UI_ACTION_OPEN_NEW_CLOSE_OLD)
		{
			UserInterfaceItem* ptr = openedUiList.top()->NewWindow();
			CloseCurrentItem();
			if (ptr != nullptr)
				openedUiList.push(ptr);
			reRenderBackground = true;
			isMenu = true;
		}

		return action;
		
	}
}

UI_ACTION_CODE MainUiController::OnCancel()
{
	if (openedUiList.empty())
	{
		openedUiList.push(new MainMenu(game));
		isMenu = true;
	}
	else
	{
		UI_ACTION_CODE action = openedUiList.top()->OnCancel();
		if (action == UI_ACTION_CLOSE_ITEM)
		{
			UserInterfaceItem* ptr = openedUiList.top();
			openedUiList.pop();
			delete ptr;
			reRenderBackground = true;

			if (openedUiList.empty())
				return UI_ACTION_CLOSE_ITEM;
		}
	}
	return UI_ACTION_NOTHING;
};

UI_ACTION_CODE MainUiController::OnKeyInput(unsigned int keyCode)
{
	if (!openedUiList.empty())
		return openedUiList.top()->OnKeyInput(keyCode);
	return UI_ACTION_NOTHING;
}

void MainUiController::RenderPlayerinfo(Player* player, uint8_t* buffer)
{

	int maxWidth = game->viewWidth;
	int maxHeight = game->viewHeight;

	int fontSize = maxHeight / 30;
	int lineSpace = fontSize;
	int paddingY = maxHeight / 30;
	int paddingX = maxWidth / 40;
	std::string anglesText = "";
	std::stringstream stream;

	int floatLength = 6;

	if (game->player.groundRotation)
	{
		int AngleDecimals = 1;
		/*Render first line: XY and XZ*/
		stream.str(std::string()); //empty string
		stream << "mouseY :" << float2str(player->angleXY, AngleDecimals, floatLength);
		stream << " mouseX :" << float2str(player->angleXZ, AngleDecimals, floatLength);
		stream << " roll :" << float2str(player->angleYZ, AngleDecimals, floatLength);

		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;

		/*Render second line: XW and ZW*/
		stream.str(std::string()); //empty string
		stream << "sMouseY:" << float2str(player->angleXW, AngleDecimals, floatLength);
		stream << " sMouseX:" << float2str(player->angleZW, AngleDecimals, floatLength);
		stream << " rollW:" << float2str(player->angleYW, AngleDecimals, floatLength);

		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;
	}

	//Render player position (Cubes-like)
	stream.str(std::string()); //empty string
	stream << "Maze position x: " << std::fixed << std::setprecision(0) << std::ceil(player->lastPos.x/game->field->roomSize);
	stream << " y: " << std::fixed << std::setprecision(0) << std::ceil(player->lastPos.y / game->field->roomSize);
	stream << " z: " << std::fixed << std::setprecision(0) << std::ceil(player->lastPos.z / game->field->roomSize);
	stream << " w: " << std::fixed << std::setprecision(0) << std::ceil(player->lastPos.w / game->field->roomSize);

	anglesText = stream.str();
	RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight - paddingY);
	paddingY += lineSpace;


	//there is no mistake - it is assignment, not comparison
	//Turn to "true" while debugging
	if (bool debug = false)
	{
		//Render player absolute position
		stream.str(std::string()); //empty string
		stream << "Coord (x,y,z,w): (" << std::fixed << std::setprecision(2);
		stream << player->lastPos.x << "," << player->lastPos.y << "," << player->lastPos.z << "," << player->lastPos.w << ")";


		anglesText = stream.str();
		RenderUItext(anglesText, fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;

		RenderUItext("vx_basis=" + vec2str(player->vx_basis) + " vx=" + vec2str(player->vx), fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;

		RenderUItext("vy_basis=" + vec2str(player->vy_basis) + " vy=" + vec2str(player->vy), fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;

		RenderUItext("vz_basis=" + vec2str(player->vz_basis) + " vz=" + vec2str(player->vz), fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;

		RenderUItext("vw_basis=" + vec2str(player->vw_basis) + " vw=" + vec2str(player->vw), fontSize, buffer, paddingX, maxHeight - paddingY);
		paddingY += lineSpace;
	}

}