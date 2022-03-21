#include "world.h"

World::World(Shader& shader, int h, int w, GLuint vert_arr, GLuint vert_buff, GLuint ele_buff)
{
	m_shader = &shader;
	height = h;
	width = w;
	vertex_array = vert_arr;
	vertex_buffer = vert_buff;
	element_buffer = ele_buff;
}
World::~World()
{
	delete m_shader;
	for (Chunk* ch : loadedChunks) {
		delete ch;
	}
}

void World::initWorld()
{
	for (int x = 0; x < 3; x++) {
		for (int z = 0; z < 3; z++) {
			Chunk* ch = new Chunk(x * xChunk, z * zChunk, *m_shader);
			loadedChunks.push_back(ch);
		}
	}
}

void World::buildWorld()
{
	for (auto ch : loadedChunks) {
		ch->buildTerrain();
	}
}
void World::renderChunks()
{
	for (auto ch : loadedChunks) {
		ch->renderChunk(height, width, vertex_array, vertex_buffer, element_buffer);
	}
}
