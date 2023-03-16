//
//  Files.cpp
//  slide
//
//  Created by Kai Wegner on 07.03.23.
//

#include "Files.hpp"
#include <filesystem>
#include <vector>
#include <cstdlib>

Files::Files(const char* basedir)
{
    std::srand(std::time(nullptr));

    for (const auto& entry : std::filesystem::recursive_directory_iterator(basedir))
    {
        if (entry.is_regular_file() && isJpegFile(entry.path().extension()))
        {
            fileNames.push_back(entry.path());
        }
    }
    count = fileNames.size();
}

std::string Files::GetNextRandom()
{
    u_long randomNumber = std::rand() % (count + 1);
    return fileNames[randomNumber].c_str();
    
}

bool Files::isJpegFile(std::string extension)
{
    return extension == ".jpg" || extension == ".jpeg";
}
Files::~Files() {
    //delete[] fileNames;
}


