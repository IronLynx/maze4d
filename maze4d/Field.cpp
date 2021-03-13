#include <Field.h>

const Cell Field::StdLightCell = Cell(LIGHT_BLOCK, 245, false);
const Cell Field::StdEmptyCell = Cell(EMPTY_BLOCK, 0, false);

Field::Field(const glm::ivec4 size, const int lightDist, Config* cfg)
	: size(size), totalSize(size.x*size.y*size.z*size.w), lightDist(lightDist)
{
	rayShader = new RaycastingShader(size, cfg);

	MapConstructor(size, lightDist);
}

void Field::MapConstructor(const glm::ivec4 size, const int lightDist)
{
	this->size = size;
	this->lightDist = lightDist;
	this->totalSize = size.x*size.y*size.z*size.w;
	curMap = new Cell_t[totalSize];
	curLightMap = new Light_t[totalSize];

	for (int i = 0; i < totalSize; i++)
	{
		curMap[i] = nullptr;
		curLightMap[i] = 0;
	}

	CreateBorders();

	rayShader->SetNewMapSize(size);

}

void Field::MapDestructor()
{
	for (int i = 0; i < totalSize; i++)
		if (curMap[i] != nullptr)
			delete curMap[i];

	delete[] curMap;
	delete[] curLightMap;
}

Field::~Field()
{
	MapDestructor();
}

void Field::NewField(const glm::ivec4 size, const int lightDist)
{
	MapDestructor();
	MapConstructor(size, lightDist);
}

void Field::Init(Config* cfg, float posX, float posY, float width, float height)
{
	rayShader->ReadConfig(cfg);
	rayShader->SetFrameSize(posX, posY, width, height);

	int memBytes = (sizeof(Cell) + sizeof(Light_t)) * (totalSize);
	Log("cubesCount: ", cubesCount, ", mem(map): ", memBytes / 1024.0f / 1024.0f, " Mb");
}



Cell Field::GetCube(int index)
{
	if (index > GetIndex(size.x - 1, size.y - 1, size.z - 1, size.w - 1))
		return Cell(0,0,false);

	if (curMap[index] == nullptr)
		return Cell(0, 0, false);

	return *curMap[index];
}

void Field::CreateCube(int index, Cell cell)
{
	if (index > GetIndex(size.x-1, size.y-1, size.z-1, size.w-1))
		return;

	isTextureValid = false;

	if (curMap[index] != nullptr)
		delete curMap[index];

	

	//clear lights
	if (cell.cellType == EMPTY_BLOCK)
	{
		curLightMap[index] = 0;
		curMap[index] = nullptr;
		return;
	}

	curMap[index] = new Cell(cell);

	if (cell.cellType == LIGHT_BLOCK)
		GenerateLight(index);
}

void Field::CreateCube(int x, int y, int z, int w, Cell cell)
{
	if (!IsCubeIndexValid(x, y, z, w))
		return;

	int index = GetIndex(x, y, z, w);
	CreateCube(index, cell);

};


void Field::CreateLightCube(int x, int y, int z, int w)
{
	int index = GetIndex(x, y, z, w);
	if (!IsCubeIndexValid(x, y, z, w))
		return;

	CreateCube(x, y, z, w, StdLightCell);

	curLightMap[index] = UINT32_MAX;

};

void Field::CreateBorders()
{
	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int z = 0; z < size.z; z++)
			{
				for (int w = 0; w < size.w; w++)
				{
					if (!(x > 0 && x < (size.x - 1) &&
						y > 0 && y < (size.y - 1) &&
						z > 0 && z < (size.z - 1) &&
						w > 0 && w < (size.w - 1)))
					{
						CreateCube(x, y, z, w);
					}
				}
			}
		}
	}
}

void Field::GenerateLight(glm::ivec4 pos)
{
	int index = GetIndex(pos.x, pos.y, pos.z, pos.w);
	GenerateLightRecursive(pos.x, pos.y, pos.z, pos.w, lightDist, -1);
	curLightMap[index] = UINT32_MAX;
	curMap[index]->cellType = LIGHT_BLOCK;
	//curMap[index].alphaValue = 1;
}

void Field::RecalculateLight()
{
	for (int i = 0; i < totalSize; i++)
			curLightMap[i] = 0;

	for (int i = 0; i < totalSize; i++)
	{
		if (curMap[i] != nullptr)
			if (curMap[i]->cellType == LIGHT_BLOCK)
				GenerateLight(i);
	}
}

void Field::GenerateLightRecursive(int px, int py, int pz, int pw, unsigned int level, int side)
{
	if (level == 0 || !IsCubeIndexValid(px, py, pz, pw))
		return;

	unsigned int scaledLevel = int((level+1) / float(lightDist) * (Texture::LIGHT_GRAD - 1));
	if (side == -1)
		scaledLevel = Texture::LIGHT_GRAD - 1;

	int index = GetIndex(px, py, pz, pw);

	//if ((curMap[index] & WALL_BLOCK) != 0)
	if (curMap[index] != nullptr)
		if (curMap[index]->cellType == WALL_BLOCK)
		{
			if (((curLightMap[index] >> (side * 4)) & (Texture::LIGHT_GRAD - 1)) < scaledLevel)
			{
				// clear light
				curLightMap[index] &= ~((Texture::LIGHT_GRAD - 1) << (side * 4));
				// set light
				curLightMap[index] |= scaledLevel << (side * 4);
				//curLightMap[index] = UINT32_MAX;
			}
			return;
		}

	if (curLightMap[index] >= level)
		return;
	curLightMap[index] = level;

	if (side != POS_X) GenerateLightRecursive(px + 1, py, pz, pw, level - 1, NEG_X);
	if (side != NEG_X) GenerateLightRecursive(px - 1, py, pz, pw, level - 1, POS_X);
	if (side != POS_Y) GenerateLightRecursive(px, py + 1, pz, pw, level - 1, NEG_Y);
	if (side != NEG_Y) GenerateLightRecursive(px, py - 1, pz, pw, level - 1, POS_Y);
	if (side != POS_Z) GenerateLightRecursive(px, py, pz + 1, pw, level - 1, NEG_Z);
	if (side != NEG_Z) GenerateLightRecursive(px, py, pz - 1, pw, level - 1, POS_Z);
	if (side != POS_W) GenerateLightRecursive(px, py, pz, pw + 1, level - 1, NEG_W);
	if (side != NEG_W) GenerateLightRecursive(px, py, pz, pw - 1, level - 1, POS_W);
}

int Field::GetIndex(const int x, const int y, const int z, const int w)
{
	assert(IsCubeIndexValid(x, y, z, w));
	return x*size.y*size.z*size.w + y*size.z*size.w + z*size.w + w;
};

int Field::GetIndex(glm::ivec4 pos)
{
	return GetIndex(pos.x, pos.y, pos.z, pos.w);
};

glm::ivec4 Field::idx4(int indexInt)
{
	int x = indexInt / (size.y*size.z*size.w);
	int x_rest = indexInt % (size.y*size.z*size.w);

	int y = x_rest / (size.z*size.w);
	int y_rest = x_rest % (size.z*size.w);

	int z = y_rest / size.w;

	int w = y_rest % size.w;
	return glm::ivec4(x, y, z, w);
}

bool Field::IsCubeIndexValid(int x, int y, int z, int w)
{
	return (
		x >= 0 && x < size.x &&
		y >= 0 && y < size.y &&
		z >= 0 && z < size.z &&
		w >= 0 && w < size.w);
}

void Field::LoadFieldToGL()
{
	RecalculateLight();

	if (!isTextureValid)
	{
		rayShader->GenerateFieldTexture(curMap, curLightMap);
		isTextureValid = true;
	}

	rayShader->LoadFieldToGL();
}

void Field::Draw(Player* player)
{
	SetCameraView(player);
	Draw();
}

void Field::Draw()
{
	if (!isTextureValid)
		LoadFieldToGL();

	rayShader->Draw();
}

//Proxy to FragmentShader
void Field::SetFrameSize(float bottomX, float bottomY, float width, float height) { rayShader->SetFrameSize(bottomX, bottomY, width, height); }
void Field::SetResolution(int viewWidth, int viewHeight) { rayShader->SetResolution(viewWidth, viewHeight); }
void Field::SetMaxRayOutDistance(int distance) { rayShader->SetMaxRayOutDistance(distance); }
void Field::SetBackgroundColor(glm::vec4  col) { rayShader->SetBackgroundColor(col.x, col.y, col.z, col.w); }
void Field::SetBackgroundColor(float R, float G, float B, float Alpha) { rayShader->SetBackgroundColor(R, G, B, Alpha); }
void Field::SetCameraView(Player* player) { rayShader->SetCameraView(player); }
void Field::SetAntialiasing(int antiAliasRate) { rayShader->SetAntialiasing(antiAliasRate); }
void Field::ReadConfig(Config* cfg) { rayShader->ReadConfig(cfg); }
