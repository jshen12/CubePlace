#ifndef WORLD_H_
#define WORLD_H_

#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "config.h"
#include "chunk.h"
#include <vector>
#include "shader.h"


class World
{
public:
	World(Shader &shader, int h, int w, GLuint vert_arr, GLuint vert_buff, GLuint ele_buff);
	~World();
	void initWorld();
	void buildWorld();
	void renderChunks();

private:
	std::vector<Chunk *> loadedChunks;
	Shader* m_shader;
	int height;
	int width;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
};


#endif 