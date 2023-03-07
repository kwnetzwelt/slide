//
//  main.cpp
//  slide
//
//  Created by Kai Wegner on 06.03.23.
//

#include <iostream>
#include <GLFW/glfw3.h>
#include "JPEGImage.hpp"



void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

double lerp(double start, double end, double fraction) {
    return start + fraction * (end - start);
}


int main(int argc, const char * argv[]) {
    // Initialize GLFW library
    glfwInit();
    
    const int width = 1920;
    const int height = 1080;
    
    // initialize timer
    auto startTime = std::chrono::high_resolution_clock::now();

    
    //std::string fileName = "/Users/kaiwegner/Downloads/1343.jpeg"; // for debugging only
    std::string fileName = "/Users/kaiwegner/Downloads/21by9.jpeg"; // for debugging only
    if(argc > 1)
    {
        fileName = std::string(argv[1]);
    }
    
    
    double elapsed = 0;
    double zoom = 0;
    double offset_x = 0;
    double offset_y = 0;
    
    
    float* correctedVertices = new float[8];
    float height_f = (float)height;
    float width_f = (float)width;
    
    
    JPEGImage image(fileName.c_str());


    // Create a window with OpenGL context
    //GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Window", glfwGetPrimaryMonitor(), NULL); // fullscreen
    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    // Set up an orthographic projection matrix to match the window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width_f, width_f, -height_f, height_f, -1.0, 1.0);

    // Switch back to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double deltaTime = std::chrono::duration<double>(delta).count();
        
        elapsed += deltaTime;
        zoom = lerp(0, 1, elapsed * 10);

        // correct vertex positions for aspect ratio
        correctedVertices[0] = -(height_f + zoom) * image.aspect; // bottom left x
        correctedVertices[1] = -(height_f + zoom); // bottom left y
        correctedVertices[2] = (height_f + zoom) * image.aspect; // bottom right x
        correctedVertices[3] = -(height_f + zoom); // bottom right y
        correctedVertices[4] = (height_f + zoom) * image.aspect; // top right x
        correctedVertices[5] = height_f + zoom; // top right y
        correctedVertices[6] = -(height_f + zoom) * image.aspect; // top left x
        correctedVertices[7] = height_f + zoom; // top left y
        
        
        float uv_bottom_left_x = 0;
        float uv_bottom_left_y = 0;
        float uv_top_right_x = 1.0f;
        float uv_top_right_y = 1.0f;
        
        // Rendering
        //glOrtho(-width/zoom, width/zoom, -height/zoom, height/zoom, -1.0, 1.0);
        //image.draw(deltaTime, width, height);

        // Load image data into a texture
        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        
        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
        
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
        
        //fullscreen image
        /*{
            //bottom left part
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-width, -height); // bottom left
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(width, -height); // bottom right
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(width, height); // top right
        }
        {
            //top right part
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-width, -height); // bottom left
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(width, height); // top right
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-width, height); // top left
        }
        */
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
