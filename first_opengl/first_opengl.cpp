#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "shader.h"


// normalized unique vertecies, z-coords are 0 so 2d
static float vertices[] = {
     // positions        // colors          // texture coords
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f    // top left 
};
// indicies for each triangle
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,  // first triangle
    1, 2, 3    // second triangle
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int main(int argc, char** argv)
{

    // instantiante glfw
    if (!glfwInit()) {
        printf("Error when instantiating glfw");
        return -1;
    }
    
    // set opengl version to 3 w/ core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // for mac users lol

    // create window object (width, height, title, fullscreen/window, share context?)
    GLFWwindow* window = glfwCreateWindow(800, 600, "First OpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to creage GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
    
    // specify dimentions (bottom left x, y, width, height)
    glViewport(0, 0, 800, 600);
    // call callback func (framebuffer_size...) when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, processInput);

    
    Shader ourShader("shader_vertex.glsl", "shader_fragment.glsl");

    // create bindable vertex array and buffers
    GLuint vertex_array, vertex_buffer, element_buffer;
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);               // generate 1 buffer w/ id vertex_buffer
    glGenBuffers(1, &element_buffer);
    glBindVertexArray(vertex_array);               // bind array first
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // write to buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);  // for element array buffers
    //glBindBuffer(GL_ARRAY_BUFFER, 0);    // for vertex array buffers
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);  // for element array buffers
    
    // Tell OpenGL how to interpret vertex buffer  (index, size(x,y,z), dtype, normalized?, stride, offset) 
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // vertex attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // wireframe mode
    
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    // load texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // generate texture(target, mipmap_level, format, width, height, 0, format, datatype, data)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture");
    }
    stbi_image_free(data);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        // draw elements
        ourShader.use();
        // render
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBindVertexArray(vertex_array);
        //glDrawArrays(GL_TRIANGLES, 0, 3);     // for vertex arrays
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  // (mode, num_indices, dtype, offset)
        // swap buffers and poll
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    
    return 0;

}