#include "Teammate.h"
std::vector<std::string> Teammate::TeammateList = { };
std::vector<std::string> Teammate::LoadTeammate(const std::string& FileName) {
    std::vector<std::string> ConfigLoaded;
    std::ifstream infile(FileName);
    if (infile.good( )) {
        std::ifstream inFile(FileName);
        if (inFile.is_open( )) {
            std::string value;
            while (inFile >> value) {
                ConfigLoaded.push_back(value);
            }
            inFile.close( );
        }
    }
    return ConfigLoaded;
}
void Teammate::SaveTeammate(std::vector<std::string> TeammateList, const std::string& FileName) {
    std::remove(FileName.c_str());
    std::ofstream outFile(FileName);
    if (outFile.is_open( )) {
        for (std::string value : TeammateList) {
            outFile << value << "\n";
        }
        outFile.close( );
    } 
}
