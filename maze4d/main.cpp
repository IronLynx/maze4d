/*-----------------------------------------------------------------------------

Четырехмерный лабиринт с видом от первого лица (визуализация 3D сечением)

Поле:
 - состоит из 4х мерных блоков (тессерактов)
 - каждый блок имеет 8 гиперграней, ограничивающих поле зрения игрока
   и формирующих стены лабиринта

Игрок:
 - может перемещаться по 4м осям
 - может вращать поле зрения по шести плоскостям (XY, XZ, XW, YZ, YW, ZW)
   Вращение не привязано к базису, т.е. выполняется всегда относительно текущего
   вектора направления

Алгоритм рендера:
 - от текущей позиции игрока проводится множество лучей в соответствии с текущим вектором
   направления взгляда игрока. Каждый луч соответствует пикселю
 - алгоритмом DDA вычисляется ближайшая непустая клетка на пути луча
 - по точке пересечения рассчитывается точка в 3D текстуре этого блока и соответствующий пиксель


 ================== Controls ==================

 Forward (+x): W,   Back (-x): S
 Up      (+y): R,   Down (-y): F
 Right   (+z): D,   Left (-z): A
 Ana     (+w): T,   Kata (-w): G

 XY (pitch): I,    K  (or mouseY)
 XZ (yaw):   L,    J  (or mouseX)
 XW:         O,    U  (or mouseY + LSHIFT)
 YZ (roll):  E,    Q
 YW:         Z,    C
 ZW:         M,    N  (or mouseX + LSHIFT)

 Reset player:      P
 Noclip:            F8
 Fullscreen:        F11
 Quit:              ESC
 Lock/unlock mouse: SPACE or mouse button

-----------------------------------------------------------------------------*/

#include <glad/glad.h> // generated from https://glad.dav1d.de
#include "shader.h"

#include <Utils.h>
#include <Game.h>

static Game game;

bool isFullscreen = false;
bool isMouseLocked = true;
glm::i32vec2 windowSize(game.viewWidth*game.viewScale, game.viewHeight*game.viewScale);
glm::i32vec2 windowPos(0, 0);

static void OnError(int error, const char* description)
{
	Log("Error code: ", error);
	CriticalError(description);
}

void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

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
			game.playerController->isMouseRotateW = false;
	}

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

	if (isMouseLocked)
	{
		if (game.playerController->isMouseRotateW)
		{
			game.player.RotateXW(float(-relY*game.playerController->mouseSens));
			game.player.RotateZW(float(relX*game.playerController->mouseSens));
		}
		else
		{
			game.player.RotateXY(float(-relY*game.playerController->mouseSens));
			game.player.RotateXZ(float(relX*game.playerController->mouseSens));
		}
	}

	lastXpos = xpos;
	lastYpos = ypos;
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

void InitScene(unsigned int& screenTex, unsigned int& VAO, Shader*& shader)
{
	// build and compile our shader zprogram
	// ------------------------------------
	shader = new Shader(
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"layout(location = 2) in vec2 aTexCoord;\n"
		"out vec3 ourColor;\n"
		"out vec2 TexCoord;\n"
		"void main(){\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"ourColor = aColor;\n"
		"TexCoord = vec2(aTexCoord.x, aTexCoord.y);}\n",

		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D texture1;\n"
		"void main(){\n"
		"FragColor = texture(texture1, TexCoord);}\n"
	);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // colors           // texture coords
		1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		1.0f,  -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	glGenTextures(1, &screenTex);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

uint8_t* InitGame(GLFWwindow* window)
{
	game.Init();

	int texDataSize = game.viewWidth * game.viewHeight * 3;
	uint8_t *texData = new uint8_t[texDataSize];
	memset(texData, 0, texDataSize);

	glfwSetKeyCallback(window, OnKeyInput);
	glfwSetMouseButtonCallback(window, OnMouseButtonInput);
	glfwSetCursorPosCallback(window, OnMouseInput);
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

void RenderFrame(double delta, GLFWwindow* window, unsigned int screenTex, unsigned int VAO, Shader* shader, uint8_t* texData)
{
	game.Render(texData);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, game.viewWidth, game.viewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);

	shader->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

int main()
{
	GLFWwindow* window = InitGL();
	uint8_t* texData = InitGame(window);

	unsigned int screenTex;
	unsigned int VAO;
	Shader* shader = nullptr;
	InitScene(screenTex, VAO, shader);

	double startFrameTime = 0, delta = 0;
	//double targetFrameTime = 0.02; // fps lock
	while (!glfwWindowShouldClose(window))
	{
		startFrameTime = glfwGetTime();
		game.playerController->Update(delta);
		RenderFrame(delta, window, screenTex, VAO, shader, texData);
		delta = glfwGetTime() - startFrameTime;
		//if (delta < targetFrameTime)
		//{
		//	Windows::Sleep(int((targetFrameTime - delta) * 1000));
		//	delta = targetFrameTime;
		//}
	}

	delete[] texData;
	delete shader;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

#ifdef _WIN32
int CALLBACK WinMain(Windows::HINSTANCE hInstance, Windows::HINSTANCE hPrevInstance, Windows::LPSTR pCmdLine, int nCmdShow)
{
	return main();
}
#endif
