#ifndef CONFIG_H_
#define CONFIG_H_


#define xChunk 16
#define yChunk 35
#define zChunk 16

#define BLOCK_RESOLUTION 16


#define NUM_CUBES  (xChunk * yChunk * zChunk)


#define speed 20.0f

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define FOV 60.0f
#define MAX_DRAW_DISTANCE 400.0f
#define MAX_CHUNK_DISTANCE 10 // # of chunks away from player (radius)

#define MAX_SELECTION_DISTANCE 5 // # block player can break/place from
#define STEPPING_DISTANCE 0.5 // # just needs to be less than 1


#endif 
