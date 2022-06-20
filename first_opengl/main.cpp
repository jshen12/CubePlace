#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <iomanip>
#include "stb_image.h"
#include "chunk.h"
#include "config.h"
#include "renderer.h"
#include "shader.h"
#include "world.h"


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


glm::vec3 cameraPos = glm::vec3(xChunk/2.0f, 25.0f, zChunk/2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
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
bool textRendered = false;

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
    else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        textRendered = !textRendered;
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

    Shader blockShader("shader_vertex.glsl", "shader_fragment.glsl");
    Shader textShader("text_vertex.glsl", "text_fragment.glsl");

    // create bindable vertex array and buffers

    GLuint vertex_arrays[2], vertex_buffers[2], element_buffers[2];   // 0 is block, 1 is text
    glGenVertexArrays(2, vertex_arrays);
    glGenBuffers(2, vertex_buffers);               // generate 1 buffer w/ id vertex_buffer
    glGenBuffers(2, element_buffers);
    //setUpBufferData(vertex_array, vertex_buffer, element_buffer);


    // generate textures
    
    GLuint textures[2];
    int width, height, nrChannels;
    
    glGenTextures(2, textures);

    glActiveTexture(GL_TEXTURE1);
    unsigned char* data = setUpTexture("font_atlas.png", width, height, nrChannels);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // generate texture(target, mipmap_level, format, width, height, 0, format, datatype, data)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    textShader.use();
    textShader.setInt("ourTexture1", 1);

    stbi_image_free(data);


    glActiveTexture(GL_TEXTURE0);

    data = setUpTexture("minecraft-textureatlas-16x16.png", width, height, nrChannels);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // generate texture(target, mipmap_level, format, width, height, 0, format, datatype, data)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    blockShader.use();
    blockShader.setInt("ourTexture1", 0);

    stbi_image_free(data);
    

    glEnable(GL_DEPTH_TEST);
    // set projection matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(FOV), float(SCREEN_WIDTH)/float(SCREEN_HEIGHT), 0.1f, MAX_DRAW_DISTANCE); // projection matrix (fov, aspect ratio, near, far)

    
    double prevTime = glfwGetTime();
    int nbFrames = 0;

    // face culling, only render front
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    //glEnable(GL_RASTERIZER_DISCARD);

    double currentFrame;
    float del;
    float lastFPS = 0.0f;
    std::stringstream text;

    World* w = new World(blockShader, height, width, vertex_arrays[0], vertex_buffers[0], element_buffers[0]);
    w->initWorld();
    w->buildWorld();
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // calculate timedelta
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        nbFrames++;
        
        // calculate FPS
        if ((currentFrame - prevTime) >= 1.0)
        {
            del = currentFrame - prevTime;
            printf("%f ms/frame FPS: %f\n", (del * 1000.0) / double(nbFrames), double(nbFrames) / (del));
            lastFPS = double(nbFrames) / (del);
            nbFrames = 0;
            prevTime = currentFrame;
        }
        // input
        processKeyboardInput(window);
        // clear buffer
        glClearColor(120.0f / 256.0f, 167.0f / 256.0f, 255.0f / 256.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw elements
        // transformations
        glm::mat4 view = glm::mat4(1.0f);   // view matrix (position camera i.e. shift objects)
        
        
        // text render
        if (textRendered) {
            text << "FPS: " << std::fixed << std::setprecision(2) << lastFPS;
            text << " Camera Vec: (" << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << ")";
            //text << "  Curr Cords: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")";
            textShader.use();
            drawText(vertex_arrays[1], vertex_buffers[1], element_buffers[1], text.str(), -0.95f, 0.95f);
            text.str(std::string());   // clear stringstream
        }

        // block render
        blockShader.use();
        // create view matrix for camera
        // (position, target (pos), up vector
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        blockShader.setMat4("matrix", projection * view);
        glBindVertexArray(vertex_arrays[0]);  // do this before drawing different elements
        w->renderChunks(cameraPos.x, cameraPos.z);

        // swap buffers and poll
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;

}