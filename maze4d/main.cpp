/*-----------------------------------------------------------------------------

First person four-dimensional maze game, visualized by 3D cross-sections.

-----------------------------------------------------------------------------*/
#pragma once

#include <glad/glad.h> // generated from https://glad.dav1d.de
#include <shader.h>
#include <Utils.h>
#include <Game.h>
#include <UserInterfaceClasses.h>
#include <MapEditor.h>

static Game game;
MainUiController mainUi = MainUiController(&game);
MapEditor editor = MapEditor(&game);
uint8_t* texData;

bool isFullscreen = false;
bool isMouseLocked = true;
bool restart = true;

glm::i32vec2 windowSize(game.viewWidth*game.viewScale, game.viewHeight*game.viewScale);
glm::i32vec2 windowPos(0, 0);

static void OnError(int error, const char* description)
{
	Log("Error code: ", error);
	CriticalError(description);
}

void processUiAction(GLFWwindow* window, UI_ACTION_CODE actionCode)
{
	switch (actionCode)
	{
	case UI_ACTION_EXIT:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case UI_ACTION_NOTHING:
		break;
	}
}

void OnKeyInputMenu(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
		mainUi.NextMenuItem();

	else if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
		mainUi.PreviousMenuItem();

	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		processUiAction(window, mainUi.OnCancel());

	else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		processUiAction(window, mainUi.OnSelect());

	else if (action == GLFW_PRESS)
		mainUi.OnKeyInput(key);
}

void OnKeyInputInGame(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	IInputController* curController = game.playerController;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		mainUi.OnCancel();
		return;
	}

	if (game.editorToolsEnabled || game.cfg->GetBool("editor_mode"))
		editor.OnKeyInput(window, key, scancode, action, mods);

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);
		if (isFullscreen)
		{
			glfwSetWindowMonitor(window, nullptr, windowPos.x, windowPos.y, windowSize.x, windowSize.y, mode->refreshRate);
		}
		else
		{
			glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
			glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
			glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			glfwSwapInterval(game.vsync);
		}
		isFullscreen = !isFullscreen;
	}

	if (isMouseLocked)
		game.playerController->OnKeyInput(window, key, scancode, action, mods);
}

void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (mainUi.isMenu)
		OnKeyInputMenu(window, key, scancode, action, mods);
	else
		OnKeyInputInGame(window, key, scancode, action, mods);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		//if (false)
	{
		if (isMouseLocked)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isMouseLocked = !isMouseLocked;
	}
}

void OnMouseButtonInput(GLFWwindow* window, int button, int action, int mods)
{
	if (game.editorToolsEnabled || game.cfg->GetBool("editor_mode"))
		if (!mainUi.isMenu)
			editor.OnMouseButtonInput(window, button, action, mods);

	//if ((button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS)
	if (false)
	{
		if (isMouseLocked)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isMouseLocked = !isMouseLocked;
	}
}

void OnMouseInput(GLFWwindow* window, double xpos, double ypos) 
{
	IInputController* curController = game.playerController;
	if (isMouseLocked && !mainUi.isMenu)
		curController->OnMouseInput(window, xpos, ypos);
	else
		curController->FreezeController();
}

void OnScrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
	if (game.editorToolsEnabled || game.cfg->GetBool("editor_mode"))
	{
		IInputController* curController = &editor;
		if (isMouseLocked && !mainUi.isMenu)
			curController->OnScrollInput(window, xoffset, yoffset);
	}
}


void OnResize(GLFWwindow* window, int width, int height)
{
	float screenRatio = (float)width / height;
	float ratio = float(game.viewWidth) / game.viewHeight;
	if (screenRatio > ratio)
	{
		int newWidth = height * game.viewWidth / game.viewHeight;
		glViewport(int((width - newWidth) / 2), 0, newWidth, height);
	}
	else
	{
		int newHeight = width * game.viewHeight / game.viewWidth;
		glViewport(0, int((height - newHeight) / 2), width, newHeight);
	}
}

GLFWwindow* InitGL()
{
	glfwSetErrorCallback(OnError);

	if (!glfwInit())
	{
		CriticalError("glfwInit failed");
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "4D maze", NULL, NULL);
	if (!window)
	{
		CriticalError("Window or OpenGL context creation failed");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(game.vsync);

	// Load all OpenGL functions using the glfw loader function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		CriticalError("Failed to initialize OpenGL context");
		return nullptr;
	}
	printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	return window;
}

uint8_t* InitGlWindow(GLFWwindow* window)
{
	int texDataSize = game.viewWidth * game.viewHeight * 4;
	uint8_t* texData = new uint8_t[texDataSize];
	memset(texData, 0, texDataSize);

	glfwSetKeyCallback(window, OnKeyInput);
	glfwSetMouseButtonCallback(window, OnMouseButtonInput);
	glfwSetCursorPosCallback(window, OnMouseInput);
	glfwSetScrollCallback(window, OnScrollInput);
	glfwSetFramebufferSizeCallback(window, OnResize);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	OnResize(window, width, height);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	windowPos.x = glm::max(mode->width / 2 - windowSize.x / 2, 0);
	windowPos.y = glm::max(mode->height / 2 - windowSize.y / 2, 0);
	glfwSetWindowPos(window, windowPos.x, windowPos.y);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return texData;
}


void RenderFrame(double delta, Shader* shader, uint8_t* texData)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (game.NeedReconfigureResolution && false)
	{
		return; //stop rendering and 
	}
	else
	{
		game.Draw();
		if (game.editorToolsEnabled || game.cfg->GetBool("editor_mode"))
			editor.Draw();
		else
			game.field->SetSelectedBlock(glm::ivec4(-1));

		if (game.cfg->GetBool("display_coords"))
			mainUi.RenderPlayerinfo(&game.player, texData);
		
		mainUi.Render(texData);
		
	}
	
	//mainUi.RenderRectangle(0, 0, 200, 200, glm::u8vec3(255, 0, 0));

}

void ReinitVideoConfig(GLFWwindow* window)
{
	game.ReinitVideoConfig();
	game.field->ReadConfig(game.cfg);
	windowSize = glm::i32vec2(game.viewWidth*game.viewScale, game.viewHeight*game.viewScale);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(monitor);
	windowPos.x = glm::max(mode->width / 2 - windowSize.x / 2, 0);
	windowPos.y = glm::max(mode->height / 2 - windowSize.y / 2, 0);
	glfwSetWindowMonitor(window, nullptr, windowPos.x, windowPos.y, windowSize.x, windowSize.y, mode->refreshRate);
	glfwSwapInterval(game.vsync);
	OnResize(window, windowSize.x, windowSize.y);
}

int main()
{
	while (restart)
	{
		restart = false;
		game.ReinitVideoConfig();		
		windowSize = glm::i32vec2(game.viewWidth*game.viewScale, game.viewHeight*game.viewScale);
		//mainUi = MainUiController(&game);
		//game.viewWidth = 800;
		//game.viewHeight = 600;
		GLFWwindow* window = InitGL();
		texData = InitGlWindow(window);		

		
		game.Init();
		editor.Init();
		//editor.OneCubeField->LoadMazeToGL();

		UserInterfaceItem::InitGL();
		
		
		int nbFrames = 0;
		double lastTime = glfwGetTime();



		double startFrameTime = 0, delta = 0;
		//double targetFrameTime = 0.02; // fps lock
		while (!glfwWindowShouldClose(window) && restart==false)
		{
			if (game.NeedReconfigureResolution)
			{
				game.NeedReconfigureResolution = false;
				ReinitVideoConfig(window);
			}

			startFrameTime = glfwGetTime();
			game.playerController->Update(delta);
			if (!game.NeedReconfigureResolution)
				RenderFrame(delta, nullptr, texData);

			glfwSwapBuffers(window);
			glfwPollEvents();

			delta = glfwGetTime() - startFrameTime;

			nbFrames++;
			double currentTime = glfwGetTime();
			if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
												 // printf and reset timer
				mainUi.FPS = (int) (nbFrames / float(currentTime - lastTime));
				nbFrames = 0;
				lastTime += 1.0;
			}

			
		}

		delete[] texData;

		glfwDestroyWindow(window);
		glfwTerminate();
		UserInterfaceItem::ClearShaders();
		editor.ClearShaders();
	}
	return 0;
}

#ifdef _WIN32
int CALLBACK WinMain(Windows::HINSTANCE hInstance, Windows::HINSTANCE hPrevInstance, Windows::LPSTR pCmdLine, int nCmdShow)
{
	return main();
}
#endif
