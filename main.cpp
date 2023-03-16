//
//  main.cpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#include <iostream>
#include <GLFW/glfw3.h>
#include "JPEGImage.hpp"
#include <GL/gl.h>
#include "Files.hpp"


static Files* imageFiles;

JPEGImage* GetNextImage()
{
    std::string fileName;
    fileName = imageFiles->GetNextRandom();
    return new JPEGImage(fileName.c_str());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void log(std::string text)
{
    std::cout << text << std::endl;
}

void PrintHelp()
{
    log("pass a directory as a first argument");
    log("pass an integer time in seconds as a second argument (default is 5)");
}

double lerp(double start, double end, double fraction) {
    return start + fraction * (end - start);
}
GLuint InitTexture(JPEGImage* image)
{
    // Load image data into a texture
    GLuint textureId;
    
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    
    return textureId;
}

int main(int argc, const char * argv[]) {
    std::string basedir;
    //std::string basedir;
    int timePerImage = 15;
    
    const int width = 1920;
    const int height = 1080;
    
    double elapsed = 0;
    double zoom = 0;
    
    GLuint textureId;
    GLFWwindow* window;
    
    
    float* correctedVertices = new float[8];
    float height_f = (float)height;
    float width_f = (float)width;
    
    // parse commandline arguments
    if(argc > 1)
    {
        basedir = std::string(argv[1]);
        if(basedir == "-h" || basedir == "--help")
            PrintHelp();
    }
    if(argc > 2)
    {
        timePerImage = std::atoi(argv[2]);
    }
    
    
    // Initialize GLFW library
    glfwInit();
    
    
    // collect files for display
    log("looking for files...");
    imageFiles = new Files(basedir.c_str());
    
    log(std::to_string(imageFiles->count) + " files found");
    


    // Create a window with OpenGL context
    window = glfwCreateWindow(width, height, "OpenGL Window", glfwGetPrimaryMonitor(), NULL); // fullscreen
    //window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Set up an orthographic projection matrix to match the window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width_f, width_f, -height_f, height_f, -1.0, 1.0);

    // Switch back to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    JPEGImage* image = GetNextImage();
    JPEGImage* next_image = GetNextImage();
    
    textureId = InitTexture(image);
    
    // initialize timer
    auto startTime = std::chrono::high_resolution_clock::now();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        
        
        if(elapsed > timePerImage)
        {
            delete image;
            image = next_image;
            next_image = GetNextImage();
            elapsed = 0;
            
            glDeleteTextures(1, &textureId);
            textureId = InitTexture(image);
            
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double deltaTime = std::chrono::duration<double>(delta).count();
        
        elapsed += deltaTime;
        zoom = lerp(0, 1, elapsed * 10);

        // correct vertex positions for aspect ratio
        correctedVertices[0] = -(height_f + zoom) * image->aspect; // bottom left x
        correctedVertices[1] = -(height_f + zoom); // bottom left y
        correctedVertices[2] = (height_f + zoom) * image->aspect; // bottom right x
        correctedVertices[3] = -(height_f + zoom); // bottom right y
        correctedVertices[4] = (height_f + zoom) * image->aspect; // top right x
        correctedVertices[5] = height_f + zoom; // top right y
        correctedVertices[6] = -(height_f + zoom) * image->aspect; // top left x
        correctedVertices[7] = height_f + zoom; // top left y
        
        
        // Enable texturing
        glEnable(GL_TEXTURE_2D);
        
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glBegin(GL_TRIANGLES);
        {
            //bottom left part
            glTexCoord2f(0, 1);
            glVertex2f(correctedVertices[0], correctedVertices[1]); // bottom left
            glTexCoord2f(1.0f, 1);
            glVertex2f(correctedVertices[2], correctedVertices[3]); // bottom right
            glTexCoord2f(1.0f, 0);
            glVertex2f(correctedVertices[4], correctedVertices[5]); // top right
        }
        {
            //top right part
            glTexCoord2f(0, 1);
            glVertex2f(correctedVertices[0], correctedVertices[1]); // bottom left
            glTexCoord2f(1.0f, 0);
            glVertex2f(correctedVertices[4], correctedVertices[5]); // top right
            glTexCoord2f(0, 0);
            glVertex2f(correctedVertices[6], correctedVertices[7]); // top left
        }
        
        glEnd();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
        
        startTime = endTime;

    }

    
    // Destroy the window and terminate GLFW library
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
