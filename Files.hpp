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
#include <mutex>

class Files{
public:
    Files(const char* filename);
    std::string GetNextRandom();
    ~Files();
    
    u_long count;
    
    

private:
    bool isJpegFile(std::string extension);
    void CollectJPGs();
    void log(std::string text);
    
    const char* directory;
    std::vector<std::filesystem::path> fileNames;
    std::mutex mutex_;
    
    
};

#endif /* Files_hpp */
