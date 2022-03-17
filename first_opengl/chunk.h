#ifndef CHUNK_H_
#define CHUNK_H_

#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "config.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cube.h"
#include "shader.h"

class Chunk
{
public:
	// startX,Z is smallest coordinate
	// so chunk spans (startX -> startX + 16), etc
	int startX;
	int startZ;	
	unsigned int numCubes;
	Chunk(int xpos, int zpos, Shader &shader);
	~Chunk();
	void getCubeFaces(int x, int y, int z, bool(&render)[6]);
	void renderChunk(int height, int width, GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer);
	void buildTerrain();
private:
	// "3-d" array of cubes
	Cube * cubes;  
	Shader * m_shader;

};


#endif 