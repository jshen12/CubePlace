#ifndef CONFIG_H_
#define CONFIG_H_


#define xChunk 16
#define yChunk 35
#define zChunk 16

#define BLOCK_RESOLUTION 16


#define NUM_CUBES  (xChunk * yChunk * zChunk)


#define speed 15.0f

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define FOV 60.0f
#define MAX_DRAW_DISTANCE 400.0f
#define MAX_CHUNK_DISTANCE 10 // # of chunks away from player (radius)

#define MAX_SELECTION_DISTANCE 4 // # block player can break/place from
#define STEPPING_DISTANCE 0.07; // stepping distance of raycast


#endif 
