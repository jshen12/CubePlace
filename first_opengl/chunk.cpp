#include "chunk.h"
#include "config.h"
#include "noise.h"

#define coordToArray(x, y, z) (x * yChunk * zChunk + y * zChunk + z)

Chunk::Chunk(int xpos, int zpos, Shader &a_shader)
{
	cubes = new Cube[xChunk * yChunk * zChunk];
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


void Chunk::getCubeFaces(int x, int y, int z, bool render[])
{
    if (!cubes[coordToArray(x, y, z)].IsActive()) 
        return;
	// bottom
	render[5] = !(y < 49 && cubes[coordToArray(x, (y + 1), z)].IsActive());
	// top
	render[4] = !(y > 0 && cubes[coordToArray(x, (y - 1), z)].IsActive());
	// right
	render[3] = !(x < 15 && cubes[coordToArray((x + 1), y, z)].IsActive());
	// left
	render[2] = !(x > 0 && cubes[coordToArray((x - 1), y, z)].IsActive());
	// back
	render[1] = !(z < 15 && cubes[coordToArray(x, y, (z + 1))].IsActive());
	// front
	render[0] = !(z > 0 && cubes[coordToArray(x, y, (z - 1))].IsActive());

}

void Chunk::renderChunk(int height, int width, GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer)
{
    Cube currCube;
    glm::mat4 model;
    glm::vec3 cubePos;
    for (int x = startX; x < xChunk + startX; x++) {
        for (int y = 0; y < yChunk; y++) {
            for (int z = startZ; z < zChunk + startZ; z++) { 
                currCube = cubes[coordToArray((x - startX), (y), (z - startZ))];
                if (currCube.IsActive()) {
                    model = glm::mat4(1.0f);
                    cubePos = glm::vec3(float(x), float(y), float(z));
                    
                    model = glm::translate(model, cubePos);
                    m_shader->setMat4("model", model);
                    glm::vec2 offset;
                    if (currCube.getType() == BlockType::BlockType_Dirt) {
                        offset = glm::vec2(2.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                        m_shader->setVec2("offset", offset);
                    }
                    else {
                        offset = glm::vec2(1.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                        m_shader->setVec2("offset", offset);
                    }

                    
                    // TODO: only run this when needed (i.e. player breaks block)

                    bool rendered[6] = { 0 };  // bitmap optimization???
                    getCubeFaces(x - startX, y, z - startZ, rendered);
                    

                    int facesCount = 0;
                    for (int i = 0; i < 6; i++)
                        facesCount += rendered[i];

                    float buffer[6 * 20] = { 0 };
                    unsigned int indices[6 * 6] = { 0 };
                    // TODO: render each face sperately for grass blocks, etc.
                    
                    
                    getBufferArray(buffer, indices, rendered);
                    
                    for (int i = 0; i < facesCount; i++) {
                        
                        if (rendered[5] && (i == facesCount - 1)) {  // top texture
                            if (currCube.getType() == BlockType::BlockType_Grass) {
                                offset = glm::vec2(13.0f / (width / BLOCK_RESOLUTION), 4.0f / (height / BLOCK_RESOLUTION));
                                m_shader->setVec2("offset", offset);
                            }
                        }
                        else {  // side texture
                            if (currCube.getType() == BlockType::BlockType_Grass) {
                                offset = glm::vec2(3.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                                m_shader->setVec2("offset", offset);
                            }
                        }
                        
                        drawBufferData(vertex_array, vertex_buffer, element_buffer, &buffer[i * 20], &indices[0],
                            sizeof(float) * 20, sizeof(unsigned int) * 6, 6);
                    }
                    
                    //drawBufferData(vertex_array, vertex_buffer, element_buffer, buffer, indices,
                     //  sizeof(float) * facesCount * 20, sizeof(unsigned int) * facesCount * 6, facesCount * 6);
                }

            }
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



