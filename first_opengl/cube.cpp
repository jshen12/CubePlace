#include "cube.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

static float vertices[] = {
    // back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    // front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    // left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     // top face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // bottom face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

};


Cube::Cube(int x, int y, int z, int t)
    :xpos(0), ypos(0), zpos(0), type(0)
{
	xpos = x;
	ypos = y;
	zpos = z;
	type = t;

}

/*
Cube::~Cube()
{
    delete xpos;
    delete ypos;
    delete zpos;
    delete type;
    delete[] renderedFaces;
}

Cube::Cube(const Cube& c)
{
    xpos = c.xpos;
    ypos = c.ypos;
    zpos = c.zpos;
    type = c.type;
    for (int i = 0; i < 6; i++)
        renderedFaces[i] = c.renderedFaces[i];
}
*/

int Cube::numRenderedFaces()
{
    int numFaces = 0;
    for (int i = 0; i < 6; i++) {
        if (renderedFaces[i])
            numFaces++;
    }
    return numFaces;
}

void getBufferArray(float* buff, unsigned int* indices, bool render[6])
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
            // for each coordinate
            for (int coord = 0; coord < 5; coord++)
            {
                *(buff++) = vertices[i * 20 + v * 5 + coord];
            }
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



