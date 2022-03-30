#ifndef WORLD_H_
#define WORLD_H_

#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <unordered_map>
#include "config.h"
#include "chunk.h"
#include <vector>
#include "shader.h"



// hash function for pair

struct pair_hash {
	//template <class T1, class T2>
	std::size_t operator () (const std::pair<int, int>& p) const {
		int idx1 = p.first / xChunk;
		int idx2 = p.second / zChunk;
		int h1 = idx1 >= 0 ? 2 * idx1 : (-2 * idx1) - 1;  // shift values to real number range
		int h2 = idx2 >= 0 ? 2 * idx2 : (-2 * idx2) - 1;
		int szudzik = h1 >= h2 ? (h1 * h1) + h1 + h2 : (h2 * h2) + h1;
		return szudzik;   // szudzik pairing function 
	}
};


class World
{
public:
	World(Shader &shader, int h, int w, GLuint vert_arr, GLuint vert_buff, GLuint ele_buff);
	~World();
	void initWorld();
	void buildWorld();
	void renderChunks(float currX, float currZ);
	void calculateFaces(int x, int y, int z, Chunk& currChunk, bool rendered[]);

private:
	std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> chunkMap;   // maps coordinates to active chunks
	Shader* m_shader;
	int height;
	int width;
	GLuint vertex_array;
	GLuint vertex_buffer;
	GLuint element_buffer;
};


#endif 