//
//  JPEGImage.hpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#ifndef JPEGImage_hpp
#define JPEGImage_hpp

#include <stdio.h>
#include <thread>
#include <atomic>
#include <string>

class JPEGImage {
public:
    int width, height;
    double aspect;
    unsigned char* data;
    std::string filename;

    JPEGImage(std::string filename);
    ~JPEGImage();

    void draw();
    void draw(double deltaTime, int screen_width, int screen_height);
    void wait();
    bool isReady();
private:
    double elapsedTime;
    std::atomic<bool> done; // Use an atomic flag.
    std::thread* initThread;
    double lerp(double start, double end, double fraction);
    void init(const char* filename);
    
};

#endif /* JPEGImage_hpp */
