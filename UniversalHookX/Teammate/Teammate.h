#pragma once
#include <iostream>
#include <vector>
#include <ostream>
#include <fstream>
#include <Windows.h>

namespace Teammate
{
    extern std::vector<std::string> TeammateList;
    extern std::vector<std::string> LoadTeammate(const std::string& FileName);
    extern void SaveTeammate(std::vector<std::string> TeammateList, const std::string& FileName);
}
