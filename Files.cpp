//
//  Files.cpp
//  slide
//
//  Created by Kai Wegner on 07.03.23.
//

#include "Files.hpp"
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <string>
#include <cstdlib>
#include <iostream>

Files::Files(const char* basedir)
{
    std::srand(std::time(nullptr));
    directory = basedir;
    std::thread t(&Files::CollectJPGs, this);
    t.detach();
}

void Files::CollectJPGs(){
    count = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file() && isJpegFile(entry.path().extension()))
        {
            std::lock_guard<std::mutex> lock(mutex_);
            fileNames.push_back(entry.path().string());
        }
    }
    count = fileNames.size();
    log(std::to_string(count) + " files found");
}
void Files::log(std::string text)
{
    std::cout << text << std::endl;
}
std::string Files::GetNextRandom()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (fileNames.empty()) {
        return "";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, fileNames.size() - 1);
    return fileNames[distrib(gen)].c_str();
 
    //u_long randomNumber = std::rand() % (count + 1);
    //return fileNames[randomNumber].c_str();
        
}

bool Files::isJpegFile(std::string extension)
{
    return extension == ".jpg" || extension == ".jpeg";
}
Files::~Files() {
    //delete[] fileNames;
}


