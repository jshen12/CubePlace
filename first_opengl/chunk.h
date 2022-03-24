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
	// startX,Z is smallest coordinate indicies
	// so chunk spans (startX * 16 -> startX * 16 + 16), etc
	int startX;
	int startZ;	
	unsigned int numCubes;
	Chunk(int xpos, int zpos, Shader &shader);
	~Chunk();
	bool doesNeedRebuild();
	void setRebuildStatus(bool status);
	void renderFaces(int height, int width, GLuint vertex_array, GLuint vertex_buffer, GLuint element_buffer, bool rendered[6], 
		int facesCount, int x, int y, int z);
	void buildTerrain();
	Cube cubeAt(int x, int y, int z);
	bool* getRendered(int x, int y, int z);
	void setRendered(int x, int y, int z, bool r[6]);
private:
	// "3-d" array of cubes
	Cube * cubes;  
	Shader * m_shader;
	bool* rendered;
	bool needsRebuild;
};


#endif 