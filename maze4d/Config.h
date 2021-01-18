#pragma once

#include <iostream>
#include <map>
#include <list> 
#include <fstream>
#include <string>
#include <algorithm>

#include <iomanip>
#include <sstream>

#include <Utils.h>

typedef int CFG_VALUE_TYPE;


#define CFG_TYPE_INT 0
#define CFG_TYPE_FLOAT 1
#define CFG_TYPE_BOOL 2

class Config
{
public:
	Config()
	{
		if (!ParseFile())
		{
			//CreateDefaultConfig();
			WriteFile();
			Log("Config not found, use default");
		}
		for (CFG_PARAM_LIST::iterator param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
		{
			Log(param->tag, ": ", param->paramValue.value);
		}

		if (!CheckConfig())
			CriticalError("Incorrect config, please delete it");
	}

	int GetInt(std::string tag)
	{
		paramValues param = GetParamByTag(tag);
		return std::stoi(param.value);
	}
	float GetFloat(std::string tag)
	{
		paramValues param = GetParamByTag(tag);
		return std::stof(param.value);
	}

	bool GetBool(std::string tag)
	{
		paramValues param = GetParamByTag(tag);
		return std::stoi(param.value);
	}


	int SetParameter(std::string paramTag, std::string value)
	{
		paramValues param = GetParamByTag(paramTag);

		if (param.valueType == CFG_TYPE_FLOAT)
			return SetParameter(paramTag, std::stof(value));

		if (param.valueType == CFG_TYPE_INT || param.valueType == CFG_TYPE_BOOL)
			return SetParameter(paramTag, std::stoi(value));

		return -3; //unknown  confign value_type
	}

	int SetParameter(std::string paramTag, float fValue)
	{

		std::stringstream stream;
		stream << std::fixed << std::setprecision(1) << fValue;
		std::string valueAsString = stream.str();

		CFG_PARAM_LIST::iterator param;
		for (param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
			if (param->tag == paramTag)
				break;

		if (param != PARAM_LIST.end())
			if (param->paramValue.valueType == CFG_TYPE_FLOAT)
				param->paramValue.value = valueAsString;
				else
				{
					if (abs(((int)round(fValue)) - fValue) > 0.001)
						return -1; //ERROR: passed value cannpt be stored as INT or bool
					else
						return SetParameter(paramTag, (int)round(fValue));
				}
		else
			return -2; //No value found

		CreateDefaultConfig();
		return 0; //success;
	}

	int SetParameter(std::string paramTag, int intValue)
	{
		std::stringstream stream;
		stream << intValue;
		std::string valueAsString = stream.str();

		CFG_PARAM_LIST::iterator param;
		for (param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
			if (param->tag == paramTag)
				break;

		//CFG_PARAM_LIST param = PARAMS.find(paramTag);
		if (param != PARAM_LIST.end())
			if (param->paramValue.valueType == CFG_TYPE_INT || param->paramValue.valueType == CFG_TYPE_FLOAT)
				param->paramValue.value = valueAsString;
			else
				if (intValue == 0 || intValue == 1)
					return SetParameter(paramTag, (intValue == 1));
				else
					return -1; //ERROR: passed value cannot be stored as BOOL parameter
		else
			return -2; //No value found

		CreateDefaultConfig();
		return 0; //success;
	}

	int SetParameter(std::string paramTag, bool bValue)
	{
		int intValue = 0;
		if (bValue) 
			intValue = 1;
		std::stringstream stream;
		stream << intValue;
		std::string valueAsString = stream.str();

		CFG_PARAM_LIST::iterator param;
		for (param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
			if (param->tag == paramTag)
				break;

		if (param != PARAM_LIST.end())
			if (param->paramValue.valueType == CFG_TYPE_BOOL)
				param->paramValue.value = valueAsString;
			else
				return -1;//ERROR: passed value cannot be stored as BOOL parameter
		else
			return -2; //No value found

		CreateDefaultConfig();
		return 0; //success
	}

	struct paramValues
	{
		//std::string tag;
		std::string category;
		CFG_VALUE_TYPE valueType;
		std::string value;
		std::string comment;
	};

	struct ConfigParam
	{
		std::string tag;
		paramValues paramValue;
	};

	typedef std::list<ConfigParam> CFG_PARAM_LIST;

	CFG_PARAM_LIST GetParamList()
	{
		return PARAM_LIST;
	}

	paramValues GetParamByTag(std::string tag)
	{
		for (CFG_PARAM_LIST::iterator param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
			if (param->tag == tag)
				return param->paramValue;

		throw std::invalid_argument("Tag does not exist");
	}

	typedef std::list<std::string> CFG_CATEGORY_LIST;

	CFG_CATEGORY_LIST GetCategoriesList()
	{
		CFG_CATEGORY_LIST result;
		for (CFG_PARAM_LIST::iterator param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
			result.push_back(param->paramValue.category);

		result.sort();
		result.unique();
		return result;
	}

	void WriteFile()
	{
		std::ofstream outputFile;
		outputFile.open("config.txt");
		if (outputFile.fail())
		{
			Log("Unable to open config file");
			return;
		}

		for (CFG_PARAM_LIST::iterator param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
		{
			outputFile << param->tag << " = " << param->paramValue.value << param->paramValue.comment << "\n";
		}
	}

private:
	bool CheckConfig()
	{
		for (CFG_PARAM_LIST::iterator param = PARAM_LIST.begin(); param != PARAM_LIST.end(); ++param)
		{
			try
			{
				if (param->paramValue.valueType == CFG_TYPE_FLOAT)
					float tmp = std::stof(param->paramValue.value);
				else
					int tmp = std::stoi(param->paramValue.value);
			}
			catch (std::exception)
			{
				return false;
			}
		}
		return true;
	}

	void CreateDefaultConfig()
	{
		//settings.clear();

		//for (std::map<std::string, paramValues>::iterator param = PARAMS.begin(); param != PARAMS.end(); ++param)
		//{
		//	settings.insert(std::pair<std::string, std::string>(param->first, param->second.value));
		//}
	}

	bool ParseFile()
	{
		std::ifstream inputFile;
		inputFile.open("config.txt");
		int initPos = 0;

		if (inputFile.fail())
		{
			Log("Unable to open config file");
			return false;
		}

		std::string line;
		while (std::getline(inputFile, line))
		{
			// Remove comment Lines
			size_t found = line.find_first_of('#');
			std::string configData = line.substr(0, found);

			// Remove ^M from configData
			configData.erase(std::remove(configData.begin(), configData.end(), '\r'), configData.end());

			if (configData.empty())
				continue;

			unsigned int length = configData.find('=');

			std::string tag, value;

			if (length != std::string::npos)
			{
				tag = configData.substr(initPos, length);
				value = configData.substr(length + 1);
			}

			// Trim white spaces
			tag = reduce(tag);
			value = reduce(value);

			if (tag.empty() || value.empty())
				continue;

			try
			{
				//any missing parameters will stay as defaults
				this->SetParameter(tag, value);
			}
			catch (std::exception)
			{
				return false;
			}
		}

		return true;
	}

	std::string trim(const std::string& str, const std::string& whitespace = " \t")
	{
		size_t strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return "";

		size_t strEnd = str.find_last_not_of(whitespace);
		size_t strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	std::string reduce(const std::string& str,
		const std::string& fill = " ",
		const std::string& whitespace = " \t")
	{
		// trim first
		std::string result = trim(str, whitespace);

		// replace sub ranges
		size_t beginSpace = result.find_first_of(whitespace);
		while (beginSpace != std::string::npos)
		{
			size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
			size_t range = endSpace - beginSpace;

			result.replace(beginSpace, range, fill);

			size_t newStart = beginSpace + fill.length();
			beginSpace = result.find_first_of(whitespace, newStart);
		}

		return result;
	}
	
	std::list<ConfigParam> PARAM_LIST{
		{ "width", { "video", CFG_TYPE_INT, "480", "" } },
		{ "height",{ "video", CFG_TYPE_INT, "360", "" } },
		{ "window_scale",{ "video", CFG_TYPE_FLOAT, "2.0", "" } },
		{ "maze_size_x",{ "game", CFG_TYPE_INT,   "3", "" } },
		{ "maze_size_y",{ "game", CFG_TYPE_INT,   "3", "" } },
		{ "maze_size_z",{ "game", CFG_TYPE_INT,   "3", "" } },
		{ "maze_size_w",{ "game", CFG_TYPE_INT,   "3", "" } },
		{ "maze_room_size",{ "video", CFG_TYPE_INT,   "8", "" } },
		{ "light_dist",{ "video", CFG_TYPE_INT,  "18", "" } },
		{ "speed",{ "controls", CFG_TYPE_FLOAT, "5.0", "" } },
		{ "mouse_sens",{ "controls", CFG_TYPE_FLOAT, "1.0", "" } },
		{ "seed",{ "game", CFG_TYPE_INT,  "-1", " # set -1 to use random seed" } },
		{ "multithreading",{ "video", CFG_TYPE_BOOL,   "0", " # 0 - disable; 1 - enable. WARNING: CPU usage can reach 100%" } },
		{ "skip_pixels",{ "video", CFG_TYPE_BOOL,   "0", " # set 1 to render all pixels each frame; 0 to render only half" } },
		{ "vsync",{ "video", CFG_TYPE_BOOL,   "0", " # 0 - disable; 1 - enable" } },
		{ "ground_rotation",{ "controls", CFG_TYPE_BOOL,   "0", " # 0 - disable; 1 - enable" } },
		{ "display_coords",{ "controls", CFG_TYPE_BOOL,   "0", " # 0 - disable; 1 - enable" } }
	};

	
};