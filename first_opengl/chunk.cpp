#include "chunk.h"
#include "config.h"
#include "noise.h"

#define coordToArray(x, y, z) (x * yChunk * zChunk + y * zChunk + z)

static const float reference_vertices[] = {
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


Chunk::Chunk(int xpos, int zpos, Shader &a_shader)
{
	cubes = new Cube[xChunk * yChunk * zChunk];
    //rendered = new bool[xChunk * yChunk * zChunk * 6];
	startX = xpos;
	startZ = zpos;
	for (int x = 0; x < xChunk; x++) {
		for (int y = 0; y < yChunk; y++) {
			for (int z = 0; z < zChunk; z++) {
				cubes[coordToArray(x, y, z)] = Cube();
			}
		}
	}
    needsRebuild = true;
    m_shader = &a_shader;
    numFaces = 0;
}

Chunk::~Chunk()
{
	delete[] cubes;
}


bool Chunk::doesNeedRebuild()
{
    return needsRebuild;
}
void Chunk::setRebuildStatus(bool status)
{
    needsRebuild = status;
}


Cube Chunk::cubeAt(int x, int y, int z)
{
    return cubes[coordToArray(x, y, z)];
}

void Chunk::deleteCube(int x, int y, int z)
{
    cubes[coordToArray(x, y, z)] = Cube();  // default not active
}

void Chunk::addCube(BlockType type, int x, int y, int z)
{
    cubes[coordToArray(x, y, z)] = Cube(type, true);
}



void Chunk::getBufferArray_1face(std::vector<float> &verts, BlockType type, int face, int height, int width, float x, float y, float z)
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
        else if (face == 4) {
            offsetX = 2.0f / (width / BLOCK_RESOLUTION);
            offsetY = 15.0f / (height / BLOCK_RESOLUTION);
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
        verts.push_back(reference_vertices[face * 20 + v * 5] + x);
        verts.push_back(reference_vertices[face * 20 + v * 5 + 1] + y);
        verts.push_back(reference_vertices[face * 20 + v * 5 + 2] + z);
        // uv tex coords
        verts.push_back(reference_vertices[face * 20 + v * 5 + 3] / BLOCK_RESOLUTION + offsetX);
        verts.push_back(reference_vertices[face * 20 + v * 5 + 4] / BLOCK_RESOLUTION + offsetY);

    }

}

void Chunk::renderFaces(std::vector<float> &verts, int height, int width, bool rendered[6], int x, int y, int z)
{
    Cube currCube = cubeAt(x - startX, y, z - startZ);
    for (int i = 0; i < 6; i++) {  // for every cube face
        if (rendered[i]) {
            getBufferArray_1face(verts, currCube.getType(), i, height, width, float(x), float(y), float(z));  
        }
    }
}



void Chunk::buildTerrain()
{
    for (int x = startX; x < xChunk + startX; x++) {
        for (int z = startZ; z < zChunk + startZ; z++) {

            float height = ((perlin(x / float(xChunk), z / float(zChunk)) + 1.0f) / 2.0f) * yChunk;
            for (int y = 0; y < int(height); y++) {
                if (y == int(height) - 1) 
                    cubes[coordToArray((x - startX), y, (z - startZ))].setType(BlockType::BlockType_Grass);
                else if (int(height) - 1 - y  < 3) 
                    cubes[coordToArray((x - startX), y, (z - startZ))].setType(BlockType::BlockType_Dirt);
                else
                    cubes[coordToArray((x - startX), y, (z - startZ))].setType(BlockType::BlockType_Stone);
                cubes[coordToArray((x - startX), y, (z - startZ))].setActive(true);
            }
            numCubes += height;
        }
    }
}



