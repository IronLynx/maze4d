#include <Utils.h>

static Random _instance;

Random* Random::GetInstance()
{
	return &_instance;
}

void Random::Init(int seed)
{
	unsigned int s = seed;
	if (seed == -1)
	{
		s = std::random_device()();
		Log("use random seed: ", s);
	}
	engine.seed(s);
}

// Generate random int in range [a,b]
int Random::GetInt(int a, int b)
{
	if (b < a)
		std::swap(a, b);
	std::uniform_int_distribution<> dist(a, b);
	return dist(engine);
}
