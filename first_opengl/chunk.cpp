#include "chunk.h"
#include "config.h"
#include "noise.h"


Chunk::Chunk(int xpos, int zpos, Shader &a_shader)
{
	cubes = new Cube[xChunk * yChunk * zChunk];
	startX = xpos;
	startZ = zpos;
	for (int x = 0; x < xChunk; x++) {
		for (int y = 0; y < yChunk; y++) {
			for (int z = 0; z < zChunk; z++) {
				if (y < 25)
					cubes[x * 50 * 16 + y * 16 + z] = Cube();
				else {
					cubes[x * 50 * 16 + y * 16 + z] = Cube();
				}
			}
		}
	}
	numCubes = 25 * 16 * 16;
    m_shader = &a_shader;
}

Chunk::~Chunk()
{
	delete[] cubes;
}


void Chunk::getCubeFaces(int x, int y, int z, bool (&render)[6])
{
    if (!cubes[x * 50 * 16 + y * 16 + z].IsActive()) 
        return;
	// bottom
	render[5] = !(y > 0 && cubes[x * 50 * 16 + (y - 1) * 16 + z].IsActive());
	// top
	render[4] = !(y < 49 && cubes[x * 50 * 16 + (y + 1) * 16 + z].IsActive());
	// right
	render[3] = !(x < 15 && cubes[(x+1) * 50 * 16 + y * 16 + z].IsActive());
	// left
	render[2] = !(x > 0 && cubes[(x-1) * 50 * 16 + y * 16 + z].IsActive());
	// back
	render[1] = !(z < 15 && cubes[x * 50 * 16 + y * 16 + (z + 1)].IsActive());
	// front
	render[0] = !(z > 0 && cubes[x * 50 * 16 + y * 16 + (z - 1)].IsActive());

}

void Chunk::renderChunk(int height, int width, GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer)
{
    for (int x = startX; x < xChunk + startX; x++) {
        for (int y = 0; y < yChunk; y++) {
            for (int z = startZ; z < zChunk + startZ; z++) {
                glm::mat4 model = glm::mat4(1.0f);
                glm::vec3 cubePos = glm::vec3(float(x), float(-y), float(z));

                model = glm::translate(model, cubePos);
                m_shader->setMat4("model", model);

                glm::vec2 offset;
                if (y == 0) {
                    offset = glm::vec2(3.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                    m_shader->setVec2("offset", offset);
                }
                else if (y < 3) {
                    offset = glm::vec2(2.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                    m_shader->setVec2("offset", offset);
                }
                else {
                    offset = glm::vec2(1.0f / (width / BLOCK_RESOLUTION), 15.0f / (height / BLOCK_RESOLUTION));
                    m_shader->setVec2("offset", offset);
                }


                // TODO: only run this when needed (i.e. player breaks block)

                bool rendered[6] = { 0 };
                getCubeFaces(x, y, z, rendered);
                int facesCount = 0;
                for (int i = 0; i < 6; i++)
                    facesCount += rendered[i];

                float buffer[6 * 20];
                unsigned int indices[6 * 6];

                getBufferArray(buffer, indices, rendered);
                drawBufferData(vertex_array, vertex_buffer, element_buffer, buffer, indices,
                    sizeof(float) * facesCount * 20, sizeof(unsigned int) * facesCount * 6, facesCount * 6);

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
                cubes[x * 50 * 16 + y * 16 + z].setActive(true);
            }
        }
    }
}



