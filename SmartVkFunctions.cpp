#include "SmartVkFunctions.h"

namespace vk
{
	size_t SmartVkFunctions::prevTabs = 0;
	bool SmartVkFunctions::enableDebugMessages = false;

	void SmartVkFunctions::error(const char* str)
	{
	}
    std::vector<const char*> SmartVkFunctions::toCharVec(std::vector<std::string> vec)
    {
        return std::vector<const char*>();
    }
    std::vector<std::string> SmartVkFunctions::addStrVectors(std::vector<std::string> vec1, std::vector<std::string> vec2)
    {
		bool notFound = true;
		for (size_t i = 0; i < vec2.size(); i++)
		{
			notFound = true;
			for (size_t j = 0; j < vec1.size() && notFound; j++)
			{
				notFound = !(vec2.at(i).compare(vec1.at(j)) == 0);
			}
			if (notFound)
			{
				vec1.push_back(vec2.at(i));
			}
		}
		return vec1;
    }
	std::vector<const char*> SmartVkFunctions::addCharVectors(std::vector<const char*> vec1, std::vector<const char*> vec2)
	{
		bool notFound = true;
		for (size_t i = 0; i < vec2.size(); i++)
		{
			notFound = true;
			for (size_t j = 0; j < vec1.size() && notFound; j++)
			{
				notFound = !(strcmp(vec2.at(i), vec1.at(j)) == 0);
			}
			if (notFound)
			{
				vec1.push_back(vec2.at(i));
			}
		}
		return vec1;
	}

	void SmartVkFunctions::destroyCharVector(std::vector<const char*> vec)
	{
		for (const char* str : vec)
		{
			delete str;
		}
	}

	void SmartVkFunctions::printDebugStr(std::string str, bool end)
	{
		printDebugStr(prevTabs, str, end);
	}
	void SmartVkFunctions::printDebugStr(size_t tabs, std::string str, bool end)
	{
		prevTabs = tabs;
		if (enableDebugMessages)
		{
			for (size_t i = 0; i < tabs; i++)
			{
				std::cout << '\t';
			}
			std::cout << str;
			if (end)
			{
				std::cout << '\n';
			}
		}
	}
}
