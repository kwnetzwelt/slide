//
//  main.cpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#include <iostream>
#include <thread>
#include <GLFW/glfw3.h>
#include "JPEGImage.hpp"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "Files.hpp"


static bool f_pressed;
static Files* imageFiles;

void log(std::string text)
{
    std::cout << text << std::endl;
}

JPEGImage* GetNextImage()
{
    std::string fileName;
    fileName = imageFiles->GetNextRandom();
    log(fileName.c_str());
    auto image = new JPEGImage(fileName);
    return image;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


void PrintHelp()
{
    log("pass a directory as a first argument");
    log("pass an integer time in seconds as a second argument (default is 6)");
    log("pass 'w' as a third argument if you want to start windowed instead of fullscreen");
    log("you can press <f> to toggle fullscreen");
    log("you can press <q> to quit");
}

double lerp(double start, double end, double fraction) {
    return start + fraction * (end - start);
}
GLuint InitTexture(JPEGImage* image, GLuint textureId)
{
    // Load image data into a texture
    //GLuint textureId;
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    
    return textureId;
}


void toggleFullscreen(GLFWwindow* window){
    GLFWmonitor* monitor = glfwGetWindowMonitor(window);
    if(monitor != NULL) {
        // switch to windowed
        glfwSetWindowMonitor(window, NULL, 80,80, 640,480, GLFW_DONT_CARE);
    }else {
        // switch to fullscreen
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primary);
        glfwSetWindowMonitor(window, primary, 0,0, mode->width, mode->height, mode->refreshRate);
    }
}

void toggleFullscreenIfKeyIsPressed(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
        f_pressed = false;
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if(!f_pressed)
        {
            toggleFullscreen(window);
        }
        f_pressed = true;
    }
}


int main(int argc, const char * argv[]) {
    std::string basedir;
    //std::string basedir;
    double timePerImage = 6;
    bool startWindowed = false;

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
    if(argc > 3)
    {
        startWindowed = std::string(argv[3]) == "w";
    }
    
    // Initialize GLFW library
    glfwInit();
    
    // we always want to display fullscreen
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

    
    
    double elapsed = 0;
    double zoom = 0;
    
    GLuint textureId;
    GLFWwindow* window;

    // collect files for display
    log("looking for files...");
    imageFiles = new Files(basedir.c_str());
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    //
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // Create a window with OpenGL context
    window = glfwCreateWindow(mode->width, mode->height, "OpenGL Window", glfwGetPrimaryMonitor(), NULL); // fullscreen
    
    // hide mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    int width = 0;
    int height = 0;

    glfwGetWindowSize(window, &width, &height);
    
    float* correctedVertices = new float[8];
    float height_f = (float)height;
    float width_f = (float)width;
    
    
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if(startWindowed) toggleFullscreen(window);
    
    
    JPEGImage* image = GetNextImage();
    image->wait();
    JPEGImage* nextImage = GetNextImage();
    glGenTextures(1, &textureId);
    InitTexture(image, textureId);
    
    // initialize timer
    auto startTime = std::chrono::high_resolution_clock::now();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
            
            
        
        toggleFullscreenIfKeyIsPressed(window);
    
        glfwGetWindowSize(window, &width, &height);
    
        height_f = (float)height;
        width_f = (float)width;
        
    
        // Set up an orthographic projection matrix to match the window size
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-width_f, width_f, -height_f, height_f, -1.0, 1.0);

        // Switch back to the modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        
        if(elapsed > timePerImage && nextImage->isReady())
        {
            delete image;
            image = nextImage;
            nextImage = GetNextImage();
            elapsed = 0;
            glfwFocusWindow(window);
            glDeleteTextures(1, &textureId);
            InitTexture(image, textureId);
        
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
        glBegin(GL_TRIANGLES);
        {
            //bottom left part
            glTexCoord2f(0, 1.0f);
            glVertex2f(correctedVertices[0], correctedVertices[1]); // bottom left
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(correctedVertices[2], correctedVertices[3]); // bottom right
            glTexCoord2f(1.0f, 0);
            glVertex2f(correctedVertices[4], correctedVertices[5]); // top right
        }
        {
            //top right part
            glTexCoord2f(0, 1.0f);
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
