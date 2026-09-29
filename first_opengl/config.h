#ifndef CONFIG_H_
#define CONFIG_H_


// world constants
#define xChunk 16
#define yChunk 60
#define zChunk 16
#define NUM_CUBES  (xChunk * yChunk * zChunk)
#define speed 15.0f

// graphics constants
#define BLOCK_RESOLUTION 16

// game constants
#define MAX_SELECTION_DISTANCE 4 // # block player can break/place from
#define STEPPING_DISTANCE 0.07; // stepping distance of raycast

// player configs
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define FOV 60.0f
#define MAX_DRAW_DISTANCE 400.0f
#define MAX_CHUNK_DISTANCE 12 // # of chunks away from player (radius)
#define QUIT_BUTTON GLFW_KEY_ESCAPE
#define WIREFRAME_BUTTON GLFW_KEY_T
#define DEBUG_BUTTON GLFW_KEY_I
#define FORWARD_BUTTON GLFW_KEY_W
#define BACKWARD_BUTTON GLFW_KEY_S
#define LEFT_BUTTON GLFW_KEY_A
#define RIGHT_BUTTON GLFW_KEY_D
#define UP_BUTTON GLFW_KEY_SPACE
#define DOWN_BUTTON GLFW_KEY_LEFT_SHIFT
#define BREAK_BLOCK_BUTTON GLFW_MOUSE_BUTTON_LEFT
#define PLACE_BLOCK_BUTTON GLFW_MOUSE_BUTTON_RIGHT
#define IS_FULLSCREEN false

#endif 
