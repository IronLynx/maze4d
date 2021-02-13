#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

//buffer with CPU rendered picture (legacy)
uniform sampler2D texture1; //Texture0

//list of 4d coordinates encoded into 3-dimensional texture
//Special function converts 4d to 3d index
//Function name: ivec3 Convert4dIdxTo3dIdx(ivec4 Idx4)
uniform sampler3D currentMap; //Texture1
uniform sampler3D currentLightMap; //Texture2

//technical parameters to help with 4d->3d conversion
uniform ivec3 MapWnAddedSize;
uniform ivec3 MapTexSize;

uniform ivec4 mapSize;
uniform ivec2 gameResolution; //viewWidth and viewHeight

uniform int CpuRender = 1;
uniform int AntiAliasingEnabled = 0;





const int  EDGES_COUNT = 8;

//regular 4d Cube texture, contains 8 textures of solid 3d cubes
uniform sampler3D edge3dCube[8]; //Textures50-57

uniform sampler3D light3dCube; //texture of the light cube 

//4d cube edge numbering convention
const int  NEG_X = 0; const int  POS_X = 1;
const int  NEG_Y = 2; const int  POS_Y = 3;
const int  NEG_Z = 4; const int  POS_Z = 5;
const int  NEG_W = 6; const int  POS_W = 7;
const int  NULL_EDGE = 8; 

//Rotation matrix
uniform vec4 vx = vec4(1.0f, 0.0f, 0.0f, 0.0f);
uniform vec4 vy = vec4(0.0f, 1.0f, 0.0f, 0.0f);
uniform vec4 vz = vec4(0.0f, 0.0f, 1.0f, 0.0f);
uniform vec4 vw = vec4(0.0f, 0.0f, 0.0f, 1.0f);

//player position
uniform vec4 pos = vec4(4.2f, 4.2f, 4.2f, 4.2f);

ivec3 Convert4dIdxTo3dIdx(ivec4 Idx4)
{
	int texWxMax = MapWnAddedSize.x;
	int texWyMax = MapWnAddedSize.y;
	int texWzMax = MapWnAddedSize.z;

	int texWz = Idx4.w / (texWxMax*texWyMax);
	int texWy = (Idx4.w % (texWxMax*texWyMax)) / texWxMax;
	int texWx = (Idx4.w % (texWxMax*texWyMax)) % texWxMax;

	int texX = texWx * mapSize.x + Idx4.x;
	int texY = texWy * mapSize.y + Idx4.y;
	int texZ = texWz * mapSize.z + Idx4.z;

	return ivec3(texX, texY, texZ);
}

vec3 rgb2hsv(vec3 c)
{
	vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	float d = q.x - min(q.w, q.y);
	float e = 1.0e-10;
	return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


ivec3 GetMapIndex(int x, int y, int z, int w)
{
	return Convert4dIdxTo3dIdx(ivec4(x,y,z,w));
}

float GetLightLevelByIndex(int edge, ivec3 idx)
{
	vec4 pixel = texelFetch(currentLightMap, idx, 0);

	int value = 0;

	if (edge == NEG_X)
		value = int(pixel.x * 255) % 16;
	if (edge == POS_X)
		value = int(pixel.x * 255) / 16;

	if (edge == NEG_Y)
		value = int(pixel.y * 255) % 16;
	if (edge == POS_Y)
		value = int(pixel.y * 255) / 16;

	if (edge == NEG_Z)
		value = int(pixel.z * 255) % 16;
	if (edge == POS_Z)
		value = int(pixel.z * 255) / 16;

	if (edge == NEG_W)
		value = int(pixel.w * 255) % 16;
	if (edge == POS_W)
		value = int(pixel.w * 255) / 16;
	
	value = value + 1; //to make it the same as CPU-based light
	return value/15.0f;
}

vec4 ApplyLight(vec4 colRGBA, float lightLevel)
{
	vec3 colHSV = rgb2hsv(colRGBA.xyz);
	float minValue = 0.15f; //Light cannot be less that 15%
	if (lightLevel < minValue)
		lightLevel = minValue;
	colHSV.z = colHSV.z * lightLevel;
	vec3 colRGB = hsv2rgb(colHSV);
	vec4 result = vec4(colRGB.x, colRGB.y, colRGB.z, colRGBA.w);

	return result;
}

bool IsCubeIndexValid(int x, int y, int z, int w)
{
	return (
		x >= 0 && x < mapSize.x &&
		y >= 0 && y < mapSize.y &&
		z >= 0 && z < mapSize.z &&
		w >= 0 && w < mapSize.w);
}

//based on x,y screen position
vec4 GetRaycastVector(vec2 texCoord)
{
	//Raycast vector and positioning
	float W2 = gameResolution.x / 2.0f;
	float H2 = gameResolution.y / 2.0f;
	float Ratio = H2 / W2;

	vec2 screenOffset = vec2(texCoord.x - 0.5f, Ratio * (texCoord.y - 0.5f));

	vec4 dX = 2 * vz * screenOffset.x;
	vec4 dY = 2 * vy * screenOffset.y;
	vec4 rayVforward = vx;

	vec4 v = rayVforward + dX + dY;

	//OpenGL can't properly work with infinity when divide to Zero 
	float epsilon = 0.00001f;
	if (v.x == 0.0f) v.x = v.x + epsilon;
	if (v.y == 0.0f) v.y = v.y + epsilon;
	if (v.z == 0.0f) v.z = v.z + epsilon;
	if (v.w == 0.0f) v.w = v.w + epsilon;

	return v;
}

vec4 GetPixelFromTexture(ivec4 map, vec4 raycastVec, int edge, ivec4 step, int blockType, float lightLevel = 1.0f)
{
	float dist = 0.0f;
	vec3 texPoint;
	vec4 v = raycastVec;

	if (edge == NEG_X || edge == POS_X)
	{
		dist = (map.x - pos.x + (1 - step.x) / 2.0f) / v.x;
		texPoint = (pos + v*dist).yzw - map.yzw;
	}
	else if (edge == NEG_Y || edge == POS_Y)
	{
		dist = (map.y - pos.y + (1 - step.y) / 2.0f) / v.y;
		texPoint = (pos + v*dist).xzw - map.xzw;
	}
	else if (edge == NEG_Z || edge == POS_Z)
	{
		dist = (map.z - pos.z + (1 - step.z) / 2.0f) / v.z;
		texPoint = (pos + v*dist).xyw - map.xyw;
	}
	else if (edge == NEG_W || edge == POS_W)
	{
		dist = (map.w - pos.w + (1 - step.w) / 2.0f) / v.w;
		texPoint = (pos + v*dist).xyz - map.xyz;
	}

	vec4 CubePixel;
	//regular block type
	if (blockType == 10)
	{
		CubePixel = texture(edge3dCube[edge], texPoint, 0);
		CubePixel = ApplyLight(CubePixel, lightLevel);
	}
	//light block type
	else
	{
		CubePixel = texture(light3dCube, texPoint);
	}

	return CubePixel;
}

// uses DDA algo (from https://lodev.org/cgtutor/raycasting.html)
vec4 GetRaycastPixel(vec4 raycastVector)
{
	
	ivec4 map = ivec4(floor(pos));
	vec4 v = raycastVector; //based on x,y screen position
	ivec4 step = ivec4(sign(v));
	vec4 deltaDist = vec4(abs(1.0f / v.x), abs(1.0f / v.y), abs(1.0f / v.z), abs(1.0f / v.w));
	vec4 sideDist = (map - pos + (1 + step) / 2.0f) * step * deltaDist; //what direction to step in x or y-direction (either +1 or -1)

	vec4 cell = vec4(0.0f, 0.0f, 0.0f, 0.0f);;
	vec4 CubePixel = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	int edge = 0;
	int outRangeDistance = 0;
	int outRangeMaxDistance = mapSize.x + mapSize.y + mapSize.z + mapSize.w;

	int blockType = 0;

	int prevEdge = -1;
	int prevBlockType = -1;
	float lightLevel = 0.0f;
	float prevLightLevel = 0.0f;

	//perform DDA
	while (true)
	{
		vec4 hitPixel = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		blockType = 0;

		//jump to next map square, OR in x-direction, OR in y-direction
		if (sideDist.x <= sideDist.y && sideDist.x <= sideDist.z && sideDist.x <= sideDist.w)
		{
			sideDist.x += deltaDist.x;
			map.x += step.x;
			edge = pos.x < map.x ? NEG_X : POS_X;
		}
		else if (sideDist.y <= sideDist.x && sideDist.y <= sideDist.z && sideDist.y <= sideDist.w)
		{
			sideDist.y += deltaDist.y;
			map.y += step.y;
			edge = pos.y < map.y ? NEG_Y : POS_Y;
		}
		else if (sideDist.z <= sideDist.x && sideDist.z <= sideDist.y && sideDist.z <= sideDist.w)
		{
			sideDist.z += deltaDist.z;
			map.z += step.z;
			edge = pos.z < map.z ? NEG_Z : POS_Z;
		}
		else if (sideDist.w < sideDist.x && sideDist.w <= sideDist.y && sideDist.w <= sideDist.z)
		{
			sideDist.w += deltaDist.w;
			map.w += step.w;
			edge = pos.w < map.w ? NEG_W : POS_W;
		}

		//check for hitPixel
		if (!IsCubeIndexValid(map.x, map.y, map.z, map.w))
		{
			outRangeDistance = outRangeDistance + 1;
			if (prevBlockType > 0)
				hitPixel = GetPixelFromTexture(map, v, edge, step, prevBlockType);

			if (outRangeDistance > outRangeMaxDistance)
				hitPixel = vec4(1.0f, 1.0f, 1.0f, 1.0f); //alpha = 1.0f
		}
		else
		{
			ivec3 mapIdx = Convert4dIdxTo3dIdx(map);
			cell = texelFetch(currentMap, mapIdx, 0);
			lightLevel = GetLightLevelByIndex(edge, mapIdx);

			blockType = int(cell.y * 255.0f);
			//Check if ray has hit a wall
			if (blockType > 0)
				hitPixel = GetPixelFromTexture(map, v, edge, step, blockType, lightLevel);
			else if (prevBlockType > 0)
				hitPixel = GetPixelFromTexture(map, v, edge, step, prevBlockType, prevLightLevel);
		}

		prevEdge = edge;
		prevBlockType = blockType;
		prevLightLevel = lightLevel;

		float Alpha = CubePixel.w;
		if (hitPixel.w > 0.0f)
			CubePixel = (hitPixel * (1.0f - Alpha) + CubePixel * Alpha);

		CubePixel.w = 1 - (1 - Alpha)* (1 - hitPixel.w);
		if (CubePixel.w >= 0.99f)
			break;
	}

	return CubePixel;
}


//1 is for x1 antialiasing (1 sample per pixel)
//2 is for x4 antialiasing (4 samples per pixel)
//3 is for x9 antialiasing (9 samples per pixel)
vec2[9] GetOffsets(int multiplicator)
{
	float pixelWidth = 1.0f / gameResolution.x;
	float pixelHeight = 1.0f / gameResolution.y;

	float offsetX = pixelWidth / 3.0f;
	float offsetY = pixelHeight / 3.0f;

	vec2 offsets[9] = vec2[9](
		vec2(0.0f, 0.0f),  // also used for x1 Aliasing (multiplicator = 1)
		vec2(-offsetX, offsetY),  // top-left
		vec2(0.0f, offsetY),  // top-center
		vec2(offsetX, offsetY),  // top-right
		vec2(-offsetX, 0.0f),  // center-left3
		vec2(offsetX, 0.0f),  // center - right
		vec2(-offsetX, -offsetY),  // bottom-left
		vec2(0.0f, -offsetY),  // bottom-center
		vec2(offsetX, -offsetY)   // bottom-right    		
		);

	if (multiplicator == 2)
	{
		offsetX = pixelWidth / 4.0f;
		offsetY = pixelHeight / 4.0f;
		offsets[0] = vec2(-offsetX, -offsetY);
		offsets[1] = vec2(-offsetX,  offsetY);
		offsets[2] = vec2( offsetX, -offsetY);
		offsets[3] = vec2( offsetX, offsetY);
	}

	return offsets;
}

void main()
{	


	if (CpuRender == 1)
	{
		FragColor = texture(texture1, TexCoord);
		return;
	}


	float pixelWidth = 1.0f / gameResolution.x;
	if (CpuRender == 2 && TexCoord.x >= 0.5f - pixelWidth)
	{
		if (TexCoord.x <= 0.5f + pixelWidth)
			FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		else
			FragColor = texture(texture1, TexCoord);
		return;
	}

	
	//Anti-aliasing x1, x 4 or x9
	int AliasMulitplicator = AntiAliasingEnabled + 1;
	if (AliasMulitplicator > 3) AliasMulitplicator = 3;
	if (AliasMulitplicator < 1) AliasMulitplicator = 1;
	
	vec4 samplePixel[9];
	vec2 offsets[9] = GetOffsets(AliasMulitplicator);
	vec4 AntialiasedPixel = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	int maxSamples = AliasMulitplicator * AliasMulitplicator;
	for (int i = 0; i < maxSamples; i++)
	{
		vec4 vSampleRay = GetRaycastVector(TexCoord + offsets[i]); //based on x,y screen position
		samplePixel[i] = GetRaycastPixel(vSampleRay);
		AntialiasedPixel += samplePixel[i] / float(maxSamples);
	}
	vec4 GamePixel = AntialiasedPixel;

	//overlay user interface texture on top of game frame
	vec4 UiPixel = texture(texture1, TexCoord);
	float UiAlpha = texture(texture1, TexCoord).a;

	FragColor = GamePixel * (1 - UiAlpha) + UiPixel*UiAlpha;
}