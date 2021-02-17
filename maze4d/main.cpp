/*-----------------------------------------------------------------------------

First person four-dimensional maze game, visualized by 3D cross-sections.


 ================== Controls ==================

 ------------ Movement ------------
 Forward (+x): W,   Back (-x): S
 Up      (+y): R,   Down (-y): F
 Right   (+z): D,   Left (-z): A
 Ana     (+w): T,   Kata (-w): G

 ------------ Rotation ------------
 XY (pitch): I,    K  (or mouseY)
 XZ (yaw):   L,    J  (or mouseX)
 XW:         O,    U  (or mouseY + LSHIFT)
 YZ (roll):  E,    Q
 YW:         Z,    C
 ZW:         M,    N  (or mouseX + LSHIFT)

 ------------ Other ------------
 Reset player:      P
 Noclip:            F8
 Fullscreen:        F11
 Lock/unlock mouse: SPACE or mouse button
 Quit:              ESC

-----------------------------------------------------------------------------*/
#pragma once

#include <glad/glad.h> // generated from https://glad.dav1d.de
#include <shader.h>

#include <Utils.h>

#include <Game.h>
#include <UserInterfaceClasses.h>

static Game game;
MainUiController mainUi = MainUiController(&game);

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

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		mainUi.OnCancel();
		return;
	}

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

	static std::array<int, 4*2> moveButtons = {
		GLFW_KEY_W, GLFW_KEY_S,
		GLFW_KEY_R, GLFW_KEY_F,
		GLFW_KEY_D, GLFW_KEY_A,
		GLFW_KEY_T, GLFW_KEY_G
	};
	static std::array<int, 6*2> rotateButtons = {
		GLFW_KEY_I, GLFW_KEY_K,
		GLFW_KEY_L, GLFW_KEY_J,
		GLFW_KEY_O, GLFW_KEY_U,
		GLFW_KEY_E, GLFW_KEY_Q,
		GLFW_KEY_Z, GLFW_KEY_C,
		GLFW_KEY_M, GLFW_KEY_N
	};

	static auto fillMove = [&](int num)
	{
		if (key == moveButtons[num])
		{
			if (action == GLFW_PRESS)
				game.playerController->isMove[num] = true;
			if (action == GLFW_RELEASE)
				game.playerController->isMove[num] = false;
		}
	};
	static auto fillRotate = [&](int num)
	{
		if (key == rotateButtons[num])
		{
			if (action == GLFW_PRESS)
				game.playerController->isRotate[num] = true;
			if (action == GLFW_RELEASE)
				game.playerController->isRotate[num] = false;
		}
	};

	for (size_t i = 0; i < moveButtons.size(); i++)
		fillMove(i);
	for (size_t i = 0; i < rotateButtons.size(); i++)
		fillRotate(i);

	if (key == GLFW_KEY_LEFT_SHIFT)
	{
		if (action == GLFW_PRESS)
			game.playerController->isMouseRotateW = true;
		else if (action == GLFW_RELEASE)
		{
			game.playerController->isMouseRotateW = false;
		}
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		game.player.AlignRotation();

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		game.player.Reset();

	if (key == GLFW_KEY_F8 && action == GLFW_PRESS)
		game.player.noclip = !game.player.noclip;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (isMouseLocked)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isMouseLocked = !isMouseLocked;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		game.player.Print();

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		game.player.ResetBasis();
	}
}

void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (mainUi.isMenu)
		OnKeyInputMenu(window, key, scancode, action, mods);
	else
		OnKeyInputInGame(window, key, scancode, action, mods);
}

void OnMouseButtonInput(GLFWwindow* window, int button, int action, int mods)
{	
	if ((button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS)
	{
		if (isMouseLocked)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		isMouseLocked = !isMouseLocked;
	}
}

void OnMouseInput(GLFWwindow* window, double xpos, double ypos) {
	static double lastXpos = 0, lastYpos = 0, relX = 0, relY = 0;
	static bool isFirstCall = true;

	if (isFirstCall)
	{
		isFirstCall = false;
		relX = 0;
		relY = 0;
	}
	else
	{
		relX = xpos - lastXpos;
		relY = ypos - lastYpos;
	}

	if (isMouseLocked && !mainUi.isMenu)
	{
		if (game.playerController->isMouseRotateW)
		{
			game.player.RotateXW(float(-relY*game.playerController->mouseSens/10));
			game.player.RotateZW(float(relX*game.playerController->mouseSens / 10));
		}
		else
		{
			game.player.RotateXY(float(-relY*game.playerController->mouseSens / 10));
			game.player.RotateXZ(float(relX*game.playerController->mouseSens / 10));
		}
	}

	lastXpos = xpos;
	lastYpos = ypos;
}

void OnScrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
	if (isMouseLocked && !mainUi.isMenu)
	{
		game.player.RotateZW(float(-yoffset*90.0f));
		//game.player.RotateZW(float(xoffset*game.playerController->mouseSens));
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
	if (game.NeedReconfigureResolution)
	{
		restart = true;
		game.NeedReconfigureResolution = false;
		mainUi.reRenderBackground = true;
		game.ReinitVideoConfig();
		return;
	}
	else
		mainUi.Render(texData);
}

int main()
{
	while (restart)
	{
		restart = false;
		game.ReinitVideoConfig();		
		windowSize = glm::i32vec2(game.viewWidth*game.viewScale, game.viewHeight*game.viewScale);
		//mainUi = MainUiController(&game);

		GLFWwindow* window = InitGL();
		uint8_t* texData = InitGlWindow(window);
		
		game.Init();
		
		int nbFrames = 0;
		double lastTime = glfwGetTime();

		double startFrameTime = 0, delta = 0;
		//double targetFrameTime = 0.02; // fps lock
		while (!glfwWindowShouldClose(window) && restart==false)
		{
			startFrameTime = glfwGetTime();
			game.playerController->Update(delta);
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
		game.ClearShaders();
	}
	return 0;
}

#ifdef _WIN32
int CALLBACK WinMain(Windows::HINSTANCE hInstance, Windows::HINSTANCE hPrevInstance, Windows::LPSTR pCmdLine, int nCmdShow)
{
	return main();
}
#endif
