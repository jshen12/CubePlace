#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "shader.h"
#include "cube.h"
#include "chunk.h"

/*
// normalized unique vertecies, z-coords are 0 so 2d
static float vertices[] = {
     // positions        // colors          // texture coords
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f    // top left 
};
*/




// translations for each cube


const int xChunk = 16;
const int yChunk = 50;
const int zChunk = 16; 

const int BLOCK_RESOLUTION = 16;

const int NUM_CUBES = xChunk*yChunk*zChunk;
glm::vec3 cubePositions[NUM_CUBES];

const float speed = 5.0f;

const unsigned int SCREEN_WIDTH = 1600;
const unsigned int SCREEN_HEIGHT = 900;
const float FOV = 60.0f;

glm::vec3 cameraPos = glm::vec3(8.0f, 2.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool currKeysDown[349];

bool firstMouse = true;
float lastX = SCREEN_WIDTH / 2;
float lastY = SCREEN_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;



bool wireframeOn = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void updateKeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0)
        return;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!wireframeOn) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   // wireframe mode
            wireframeOn = true;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            wireframeOn = false;
        }
    }
    else if (action == GLFW_PRESS)
        currKeysDown[key] = true;
    else if (action == GLFW_RELEASE)
        currKeysDown[key] = false;
    else
        return;
}

void processKeyboardInput(GLFWwindow* window)
{

    float cameraSpeed = speed * deltaTime;
    if (currKeysDown[GLFW_KEY_W]) {
        glm::vec3 newCam = glm::vec3(cameraFront.x, 0, cameraFront.z);
        cameraPos += glm::normalize(newCam) * cameraSpeed;
    }
    if (currKeysDown[GLFW_KEY_S]) {
        glm::vec3 newCam = glm::vec3(cameraFront.x, 0, cameraFront.z);
        cameraPos -= glm::normalize(newCam) * cameraSpeed;
    }
    if (currKeysDown[GLFW_KEY_A])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (currKeysDown[GLFW_KEY_D])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (currKeysDown[GLFW_KEY_SPACE])
        cameraPos += glm::vec3(0, 1, 0) * cameraSpeed;
    if (currKeysDown[GLFW_KEY_LEFT_SHIFT])
        cameraPos-= glm::vec3(0, 1, 0) * cameraSpeed;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;


    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void drawBufferData(GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer, 
    static float *vert, static unsigned int *ind, 
    int vert_size, int ind_size, int count)
{
    glBindVertexArray(vertex_array);               // bind array first
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);  // bind to gl_array_buffer
    glBufferData(GL_ARRAY_BUFFER, vert_size, vert, GL_STATIC_DRAW);  // write to buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);    // for vertex array buffers
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_size, ind, GL_STATIC_DRAW);
    // Tell OpenGL how to interpret vertex buffer  (index, size(x,y,z), dtype, normalized?, stride, offset) 
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

unsigned char* setUpTexture(const char* path, int &width, int &height, int &nrChannels)
{
    
    // load texture
    stbi_set_flip_vertically_on_load(true);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // fix alignment issues
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data)
        printf("Failed to load texture");
    return data;
}

void bindTexture(GLuint texture, unsigned char* data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // generate texture(target, mipmap_level, format, width, height, 0, format, datatype, data)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void extractTextureAtlas(unsigned char* input, unsigned char* output, int row_idx, int col_idx)
{

}


int main(int argc, char** argv)
{


    for (int i = 0; i < 348; i++) {
        currKeysDown[i] = false;
    }

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
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "First OpenGL", NULL, NULL);
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
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // call callback func (framebuffer_size...) when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, updateKeyboardInput);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    Shader ourShader("shader_vertex.glsl", "shader_fragment.glsl");

    // create bindable vertex array and buffers
    
    GLuint vertex_array, vertex_buffer, element_buffer;
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);               // generate 1 buffer w/ id vertex_buffer
    glGenBuffers(1, &element_buffer);
    //setUpBufferData(vertex_array, vertex_buffer, element_buffer);
    
    
    // generate textures
    unsigned int texture1;
    glGenTextures(1, &texture1);
    int width, height, nrChannels;
    //unsigned char* data = setUpTexture("pixel-1x1.png", width, height, nrChannels);

    unsigned char* data = setUpTexture("minecraft-textureatlas-16x16.png", width, height, nrChannels);
    
    unsigned char* block_texture;
    block_texture = new unsigned char[BLOCK_RESOLUTION * BLOCK_RESOLUTION * nrChannels] ;

    int startCol = BLOCK_RESOLUTION * nrChannels * 2;
    int startRow = BLOCK_RESOLUTION * nrChannels * 2;
    int counter = 0;
    printf("%d %d %d\n", width, height, nrChannels);
    // row section
    for (int y = startRow; y < startRow + BLOCK_RESOLUTION; y++) {
        // column section
        for (int x = startCol; x < startCol + BLOCK_RESOLUTION; x++) {
            for (int c = 0; c < nrChannels; c++) {
                block_texture[counter] = data[y * width * nrChannels + x * nrChannels + c];
                counter++;
            }
        }
    }
    
    
    bindTexture(texture1, block_texture, width=BLOCK_RESOLUTION, height=BLOCK_RESOLUTION);
    //bindTexture(texture1, data, width = width, height = height);
    delete[]block_texture;
  
    ourShader.use();
    ourShader.setInt("ourTexture1", 0);
    glEnable(GL_DEPTH_TEST);

    // set projection matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(FOV), float(SCREEN_WIDTH)/float(SCREEN_HEIGHT), 0.1f, 100.0f); // projection matrix (fov, aspect ratio, near, far)
    ourShader.setMat4("projection", projection);

    
    double prevTime = glfwGetTime();
    int nbFrames = 0;

    // render loop
    
    while (!glfwWindowShouldClose(window))
    {
        // calculate timedelta
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        nbFrames++;

        // calculate FPS
        if ((currentFrame - prevTime) >= 1.0)
        {
            float del = currentFrame - prevTime;
            printf("%f ms/frame FPS: %f\n", (del * 1000.0) / double(nbFrames), double(nbFrames) / (del));
            nbFrames = 0;
            prevTime = currentFrame;
        }
        // input
        processKeyboardInput(window);

        // clear buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        ourShader.use();
        //printf("x: %f y: %f z: %f\n", cameraPos.x, cameraPos.y, cameraPos.z);
        // draw elements
        // transformations
        glm::mat4 view = glm::mat4(1.0f);   // view matrix (position camera i.e. shift objects)
        // create view matrix for camera
        // (position, target (pos), up vector
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);    
        ourShader.setMat4("view", view);
        // render
        glBindVertexArray(vertex_array);  // do this before drawing different elements

        Chunk* ch = new Chunk(0, 0);
        for (int x = ch->startX; x < xChunk + ch->startX; x++) {
            for (int y = 0; y < yChunk; y++) {
                for (int z = ch->startZ; z < zChunk + ch->startZ; z++) {
                    glm::mat4 model = glm::mat4(1.0f);
                    glm::vec3 cubePos = glm::vec3(float(x), float(-y), float(z));
                    
                    model = glm::translate(model, cubePos);
                    ourShader.setMat4("model", model);

                    bool rendered[6];
                    ch->getCubeFaces(x, y, z, rendered);
                    int facesCount = 0;
                    for (int i = 0; i < 6; i++)
                        facesCount += rendered[i];
                    
                    float buffer[6 * 20];
                    unsigned int indices[6 * 6];
                    
                    getBufferArray(buffer, indices, rendered);                    
                    drawBufferData(vertex_array, vertex_buffer, element_buffer, buffer, indices, 
                        sizeof(float) * facesCount * 20, sizeof(unsigned int) * facesCount * 6, facesCount * 6);

                }
            }
        }
        
        
        // swap buffers and poll
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();

    
    return 0;

}