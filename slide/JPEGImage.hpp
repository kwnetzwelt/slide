//
//  JPEGImage.hpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#ifndef JPEGImage_hpp
#define JPEGImage_hpp

#include <stdio.h>

class JPEGImage {
public:
    int width, height;
    double aspect;
    unsigned char* data;

    JPEGImage(const char* filename);
    ~JPEGImage();

    void draw();
    void draw(double deltaTime, int screen_width, int screen_height);
private:
    double elapsedTime;
    
    double lerp(double start, double end, double fraction);
    
    
};

#endif /* JPEGImage_hpp */
