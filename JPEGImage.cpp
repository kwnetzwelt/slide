//
//  JPEGImage.cpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#include "JPEGImage.hpp"
#include <jpeglib.h>
#include <cmath>
#include <iostream>
#include <chrono>

/*extern "C" {
    #include <jpeglib.h>
}*/
#ifdef __APPLE__
#include <OpenGL/gl.h> // for building on mac
#elif __linux__
#include <GL/gl.h>
#endif
#include <stdio.h>

JPEGImage::JPEGImage(std::string filename) {
    
    done = false;
    this->filename = filename;
    initThread = new std::thread([this] {
        init(this->filename.c_str());
        done = true;
    });

}

bool JPEGImage::isReady() {
    // Get thread status using wait_for as before.
    if(done && initThread->joinable())
    {
        initThread->join();
    }
    return done;
}

void JPEGImage::wait() {
    while(!isReady())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void JPEGImage::init(const char* filename){ //, int* width, int* height, double* aspect, unsigned char* data) {
    elapsedTime = 0;
    FILE* infile = fopen(filename, "rb");

    if (!infile) {
        printf("Failed to open JPEG file: %s\n", filename);
        return;
    }

    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    std::cout << "jpeg color space: " << cinfo.jpeg_color_space << "\n";
    std::cout << "out color space: " << cinfo.out_color_space << "\n";
    std::cout << "dct method: " << cinfo.dct_method << "\n";
    

    width = cinfo.output_width;
    height = std::fmax(1,cinfo.output_height);
    aspect = width/(double)height;
    unsigned int numChannels = cinfo.num_components;
    data = new unsigned char[width * height * numChannels];

    unsigned char* rowptr;
    while (cinfo.output_scanline < height )
    {
        rowptr = data + cinfo.output_scanline * width * numChannels;
        jpeg_read_scanlines( &cinfo, &rowptr, 1 );
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
}

JPEGImage::~JPEGImage() {
    delete[] data;
    delete initThread;
}

void JPEGImage::draw() {
    glPixelZoom(1920 / (float)width, -1080 / (float)height);
    glRasterPos2i(-1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void JPEGImage::draw(double deltaTime, int screen_width, int screen_height) {
    
    double window_aspect = screen_width/(double)screen_height;
    
    elapsedTime += deltaTime;
    //const double speed = 0.01;
    //double frac = elapsedTime * speed;
    
    double scale_x = 0;
    double scale_y = 0;
    if(window_aspect > aspect)
    {
        scale_x = (screen_width-width)/2.0;
        glPixelZoom(screen_width / (float)width, -screen_height / (float)height);
        
    }else
    {
        scale_y = (screen_height-height)/2.0;
        double zoom =-screen_height / (float)height;
        glPixelZoom(screen_width *abs(zoom), zoom);
        
    }
    glRasterPos2d(scale_x,scale_y);
    
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
}

double JPEGImage::lerp(double start, double end, double fraction) {
    return start + fraction * (end - start);
}
