#pragma once

#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>

namespace vk
{
	class SmartVkFunctions
	{
	public:
		static size_t prevTabs;
		static bool enableDebugMessages;

		static void error(const char* str);

		static std::vector<const char*> toCharVec(std::vector<std::string> vec);
		static std::vector<std::string> addStrVectors(std::vector<std::string> vec1, std::vector<std::string> vec2);
		static std::vector<const char*> charVecIntersect(std::vector<const char*> vec1, std::vector<const char*> vec2);
		static std::vector<const char*> addCharVectors(std::vector<const char*> vec1, std::vector<const char*> vec2);

		static void destroyCharVector(std::vector<const char*> vec);

		static void printDebugStr(std::string str, bool end = true);
		static void printDebugStr(size_t tabs, std::string str, bool end = true);
	};
}
