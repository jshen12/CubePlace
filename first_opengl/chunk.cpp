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
    vertices = {};
    indCount = 0;
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


void Chunk::clearVectors()
{
    vertices.clear();
    indices.clear();
    indCount = 0;
}

void Chunk::getBufferArray_1face(BlockType type, int face, int height, int width, float x, float y, float z)
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
        vertices.push_back(reference_vertices[face * 20 + v * 5] + x);
        vertices.push_back(reference_vertices[face * 20 + v * 5 + 1] + y);
        vertices.push_back(reference_vertices[face * 20 + v * 5 + 2] + z);
        // uv tex coords
        vertices.push_back(reference_vertices[face * 20 + v * 5 + 3] / BLOCK_RESOLUTION + offsetX);
        vertices.push_back(reference_vertices[face * 20 + v * 5 + 4] / BLOCK_RESOLUTION + offsetY);

    }

    indices.push_back(indCount);
    indices.push_back(indCount + 1);
    indices.push_back(indCount + 2);
    indices.push_back(indCount + 2);
    indices.push_back(indCount + 3);
    indices.push_back(indCount);

    indCount += 4;

}

void Chunk::renderFaces(int height, int width, bool rendered[6], int x, int y, int z)
{
    glm::mat4 model;
    glm::vec3 cubePos;
    
    Cube currCube = cubeAt(x - startX, y, z - startZ);

    model = glm::mat4(1.0f);
    cubePos = glm::vec3(float(x), float(y), float(z));

    for (int i = 0; i < 6; i++) {  // for every cube face
        if (rendered[i]) {
            getBufferArray_1face(currCube.getType(), i, height, width, float(x), float(y), float(z));            
        }
    }

}

void Chunk::drawMesh(GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer)
{
    drawBufferData(vertex_array, vertex_buffer, element_buffer, &vertices[0], &indices[0],
        sizeof(float) * vertices.size(), sizeof(unsigned int) * indices.size(), indices.size());
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



