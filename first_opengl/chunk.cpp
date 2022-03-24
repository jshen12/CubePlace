#include "chunk.h"
#include "config.h"
#include "noise.h"

#define coordToArray(x, y, z) (x * yChunk * zChunk + y * zChunk + z)
#define coordToBool(x, y, z) (x * yChunk * zChunk * 6 + y * zChunk * 6 + z * 6)

Chunk::Chunk(int xpos, int zpos, Shader &a_shader)
{
	cubes = new Cube[xChunk * yChunk * zChunk];
    rendered = new bool[xChunk * yChunk * zChunk * 6];
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
	numCubes = 0;
    m_shader = &a_shader;
}

Chunk::~Chunk()
{
	delete[] cubes;
    delete[] rendered;
    delete m_shader;
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

bool* Chunk::getRendered(int x, int y, int z)
{
    return &rendered[coordToBool(x, y, z)];
}

void Chunk::setRendered(int x, int y, int z, bool r[6])
{

    for (int i = 0; i < 6; i++) {
        rendered[coordToBool(x, y, z) + i] = r[i];
    }
}

void Chunk::renderFaces(int height, int width, GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer, bool rendered[6],
    int facesCount, int x, int y, int z)
{
    glm::mat4 model;
    glm::vec3 cubePos;
    
    Cube currCube = cubeAt(x - startX, y, z - startZ);
    float buffer[20] = { 0 };
    unsigned int indices[6] = { 0 };

    //getBufferArray(buffer, indices, currCube.getType(), rendered);
    model = glm::mat4(1.0f);
    cubePos = glm::vec3(float(x), float(y), float(z));

    model = glm::translate(model, cubePos);
    m_shader->setMat4("model", model);
    
    for (int i = 0; i < 6; i++) {
        if (rendered[i]) {
            getBufferArray_1face(buffer, indices, currCube.getType(), i, height, width);
            drawBufferData(vertex_array, vertex_buffer, element_buffer, buffer, indices,
                sizeof(float) * 20, sizeof(unsigned int) * 6, 6);
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



