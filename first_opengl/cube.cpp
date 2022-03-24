#include "cube.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "config.h"

// start bottom left, counter-clockwise
static const float vertices[] = {
    // back face 
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    // front face
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // left face
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    // right face
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     // bottom face
     -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     // top face
     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f

};


Cube::Cube()
{
    isActive = false;
    type = BlockType::BlockType_Default;

}

Cube::Cube(BlockType t, bool active)
{
    isActive = active;
    type = t;
}


Cube::~Cube()
{
   
}

bool Cube::IsActive()
{
    return isActive;
}

void Cube::setActive(bool active)
{
    isActive = active;
}

BlockType Cube::getType()
{
    return type;
}

void Cube::setType(BlockType t)
{
    type = t;
}


void getBufferArray(float* buff, unsigned int* indices, BlockType type, bool render[])
{
    
    int face_count = 0;
	// for each face
    for (int i = 0; i < 6; i++)
    {
        // dont write if not rendered
        if (!render[i]) 
            continue;

        // for each vertecies in a face
        for (int v = 0; v < 4; v++)
        {
            // vertecies
            *(buff++) = vertices[i * 20 + v * 5];
            *(buff++) = vertices[i * 20 + v * 5 + 1];
            *(buff++) = vertices[i * 20 + v * 5 + 2];
            // uv tex coords
            *(buff++) = vertices[i * 20 + v * 5 + 3] / BLOCK_RESOLUTION;
            *(buff++) = vertices[i * 20 + v * 5 + 4] / BLOCK_RESOLUTION;

        }
        *(indices++) = face_count * 4;
        *(indices++) = face_count * 4 + 1;
        *(indices++) = face_count * 4 + 2;
        *(indices++) = face_count * 4 + 2;
        *(indices++) = face_count * 4 + 3;
        *(indices++) = face_count * 4;

        face_count++;

    }
}

void getBufferArray_1face(float* buff, unsigned int* indices, BlockType type, int face, int height, int width)
// 0: back 1: front 2: left 3: right 4: bottom 5: top
{

    float offsetX;
    float offsetY;
    if (type == BlockType::BlockType_Dirt) {
        offsetX = 2.0f / (width / BLOCK_RESOLUTION);
        offsetY = 15.0f / (height / BLOCK_RESOLUTION);
    }
    else if (type == BlockType::BlockType_Grass) {
        if (face == 5) {
            offsetX = 13.0f / (width / BLOCK_RESOLUTION);
            offsetY = 4.0f / (height / BLOCK_RESOLUTION);
        }
        else {
            offsetX = 3.0f / (width / BLOCK_RESOLUTION);
            offsetY = 15.0f / (height / BLOCK_RESOLUTION);
        }
    }
    else {
        offsetX = 1.0f / (width / BLOCK_RESOLUTION);
        offsetY = 15.0f / (height / BLOCK_RESOLUTION);
    }
    // for each vertecies in a face
    for (int v = 0; v < 4; v++)
    {
        // vertecies
        *(buff++) = vertices[face * 20 + v * 5];
        *(buff++) = vertices[face * 20 + v * 5 + 1];
        *(buff++) = vertices[face * 20 + v * 5 + 2];
        // uv tex coords
        *(buff++) = vertices[face * 20 + v * 5 + 3] / BLOCK_RESOLUTION + offsetX;
        *(buff++) = vertices[face * 20 + v * 5 + 4] / BLOCK_RESOLUTION + offsetY;

    }

    *(indices++) = 0;
    *(indices++) = 1;
    *(indices++) = 2;
    *(indices++) = 2;
    *(indices++) = 3;
    *(indices++) = 0;
    
}



