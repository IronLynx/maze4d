#pragma once

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>

#include <Utils.h>

class Config
{
public:
	Config()
	{
		if (!ParseFile())
		{
			CreateDefaultConfig();
			WriteFile();
			Log("Config not found, use default");
		}
		for (std::map<std::string, paramValues>::iterator param = PARAMS.begin(); param != PARAMS.end(); ++param)
		{
			Log(param->first, ": ", settings.find(param->first)->second);
		}

		if (!CheckConfig())
			CriticalError("Incorrect config, please delete it");
	}

	int GetInt(std::string tag)
	{
		std::map<std::string, std::string>::iterator paramValueIter = settings.find(tag);
		return std::stoi((paramValueIter->second));
	}
	float GetFloat(std::string tag)
	{
		std::map<std::string, std::string>::iterator paramValueIter = settings.find(tag);
		return std::stof((paramValueIter->second));
	}

private:
	bool CheckConfig()
	{
		std::map<std::string, std::string>::iterator paramValueIter;

		for (std::map<std::string, paramValues>::iterator param = PARAMS.begin(); param != PARAMS.end(); ++param)
		{
			paramValueIter = settings.find(param->first);

			if (paramValueIter == settings.end())
			{
				return false;
			}

			try
			{
				if (param->second.isFloat)
					float tmp = std::stof(paramValueIter->second);
				else
					int tmp = std::stoi(paramValueIter->second);
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
		settings.clear();

		for (std::map<std::string, paramValues>::iterator param = PARAMS.begin(); param != PARAMS.end(); ++param)
		{
			settings.insert(std::pair<std::string, std::string>(param->first, param->second.defaultValue));
		}
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

		for (std::map<std::string, paramValues>::iterator param = PARAMS.begin(); param != PARAMS.end(); ++param)
		{
			outputFile << param->first << " = " << param->second.defaultValue << param->second.comment << "\n";
		}
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

			// Check if any of the tags is repeated more than one times
			// it needs to pick the latest one instead of the old one.

			// Search, if the tag is already present or not
			// If it is already present, then delete an existing one

			std::map<std::string, std::string>::iterator itr = settings.find(tag);
			if (itr != settings.end())
			{
				settings.erase(tag);
			}

			settings.insert(std::pair<std::string, std::string>(tag, value));
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


	std::map<std::string, std::string> settings;

	struct paramValues
	{
		bool isFloat;
		std::string defaultValue;
		std::string comment;
	};

	std::map<std::string, paramValues> PARAMS {
		{ "width",         { false, "640", "" } },
		{ "height",        { false, "360", "" } },
		{ "window_scale",  {  true, "2.0", "" } },
		{ "maze_size_x",   { false,   "3", "" } },
		{ "maze_size_y",   { false,   "3", "" } },
		{ "maze_size_z",   { false,   "3", "" } },
		{ "maze_size_w",   { false,   "3", "" } },
		{ "maze_room_size",{ false,   "8", "" } },
		{ "light_dist",    { false,  "18", "" } },
		{ "speed",         {  true, "5.0", "" } },
		{ "mouse_sens",    {  true, "0.1", "" } },
		{ "seed",          { false,  "-1", " # set -1 to use random seed" } },
		{ "multithreading",{ false,   "0", " # 0 - disable; 1 - enable. WARNING: CPU usage can reach 100%" } },
		{ "skip_pixels",   { false,   "0", " # set 1 to render all pixels each frame; 0 to render only half" } },
		{ "vsync",         { false,   "0", " # 0 - disable; 1 - enable" } }
	};
};