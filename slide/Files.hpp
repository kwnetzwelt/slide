//
//  Files.hpp
//  slide
//
//  Created by Kai Wegner on 07.03.23.
//

#ifndef Files_hpp
#define Files_hpp

#include <stdio.h>
#include <vector>
#include <filesystem>

class Files{
public:
    u_long count;
    Files(const char* filename);
    std::string GetNextRandom();
    ~Files();

private:
    std::vector<std::filesystem::path> fileNames;
    bool isJpegFile(std::string extension);
};

#endif /* Files_hpp */
