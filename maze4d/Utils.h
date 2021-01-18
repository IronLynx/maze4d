#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <glm/trigonometric.hpp>
#include <glm/exponential.hpp>
#include <glm/common.hpp>
#include <glm/packing.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>
#include <glm/integer.hpp>
#include <fstream>

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/ext.hpp>

#ifdef _WIN32
namespace Windows
{
#undef APIENTRY
#include <Windows.h>
#undef min
#undef max
}
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <array>
#include <vector>
#include <stdexcept>
#include <random>

#define NEG_X 0
#define POS_X 1
#define NEG_Y 2
#define POS_Y 3
#define NEG_Z 4
#define POS_Z 5
#define NEG_W 6
#define POS_W 7
#define NULL_EDGE 8
#define EDGES_COUNT 8

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define AXIS_W 3

#define cosd(x) (glm::cos(glm::radians(x)))
#define sind(x) (glm::sin(glm::radians(x)))


template <typename T>
static float angleBetweenVecs(T& a, T& b)
{
	T da = glm::normalize(a);
	T db = glm::normalize(b);
	return glm::degrees(glm::acos(glm::dot(da, db)));
}

static void PrintVec(glm::vec2& v)
{
	std::cout << v.x << ' ' << v.y << " (len:" << glm::length(v) << ')' << std::endl;
}
static void PrintVec(glm::vec3& v)
{
	std::cout << v.x << ' ' << v.y << ' ' << v.z << " (len:" << glm::length(v) << ')' << std::endl;
}
static void PrintVec(glm::vec4& v)
{
	std::cout << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w << " (len:" << glm::length(v) << ')' << std::endl;
}

template<typename T>
static void Print(T t)
{
	std::cout << t << std::endl;
}

template<typename T>
static void PrintN(T t)
{
	/*
	std::ofstream outfile;

	outfile.open("Diagtostics.txt", std::ios_base::app); // append instead of overwrite
	outfile << t;	
	*/
	std::cout << t;
}

// Base case, no args
static void _Log() {}

template <typename T, typename...Ts>
static void _Log(T &&first, Ts&&... rest) {
	// print it
	PrintN(std::forward<T>(first));
	// Forward the rest.
	_Log(std::forward<Ts>(rest)...);
}

// Split the parameter pack.
// We want the first argument, so we can print it.
// And the rest so we can forward it to the next call to f
template <typename T, typename...Ts>
static void Log(T &&first, Ts&&... rest) {
	_Log(first, rest...);
	PrintN('\n');
}
template <typename T, typename...Ts>
static void LogN(T &&first, Ts&&... rest) {
	_Log(first, rest...);
}

static void CriticalError(const char* s)
{
	Log(s);
#ifdef _WIN32
	Windows::MessageBoxA(NULL, s, "Error", MB_OK | MB_ICONERROR);
#else
	fprintf(stderr, "Error: %s", s);
#endif
	glfwTerminate();
	exit(-1);
}

class Random
{
public:
	Random() {}
	void Init(int seed = -1);
	static Random* GetInstance();

	int GetInt(int a, int b);

private:
	std::default_random_engine engine;
};

static void HSVtoRGB(int H, int S, int V, glm::u8& R, glm::u8& G, glm::u8& B) {
	assert(!(H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0));

	float s = S / 100.0f;
	float v = V / 100.0f;
	float C = s*v;
	float X = C * float(1 - abs(fmod(H / 60.0f, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	R = glm::u8((r + m) * 255);
	G = glm::u8((g + m) * 255);
	B = glm::u8((b + m) * 255);
}
