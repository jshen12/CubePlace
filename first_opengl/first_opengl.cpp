#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "chunk.h"
#include "config.h"
#include "renderer.h"
#include "shader.h"

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
glm::vec3 cubePositions[NUM_CUBES];

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
    GLuint textures;
    glGenTextures(1, &textures);
    int width, height, nrChannels;

    unsigned char* data = setUpTexture("minecraft-textureatlas-16x16.png", width, height, nrChannels);
    loadTexture(textures, data, width = width, height = height);

    ourShader.use();
    ourShader.setInt("ourTexture1", 0);
    ourShader.setInt("TexSize", BLOCK_RESOLUTION);
    glEnable(GL_DEPTH_TEST);

    stbi_image_free(data);

    // set projection matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(FOV), float(SCREEN_WIDTH)/float(SCREEN_HEIGHT), 0.1f, 100.0f); // projection matrix (fov, aspect ratio, near, far)
    ourShader.setMat4("projection", projection);

    
    double prevTime = glfwGetTime();
    int nbFrames = 0;

    Chunk* ch = new Chunk(0, 0, ourShader);
    ch->buildTerrain();
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures);
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


        
        ch->renderChunk(height, width, vertex_array, vertex_buffer, element_buffer);

        
        // swap buffers and poll
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();

    
    return 0;

}