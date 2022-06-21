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
#include <vector>

class Chunk
{
public:
	// startX,Z is smallest coordinate indicies
	// so chunk spans (startX * 16 -> startX * 16 + 16), etc
	int startX;
	int startZ;	
	unsigned int numCubes;
	unsigned int numFaces;
	Chunk(int xpos, int zpos, Shader &shader);
	~Chunk();
	bool doesNeedRebuild();
	void setRebuildStatus(bool status);
	void renderFaces(std::vector<float> &verts, std::vector<unsigned int> &inds, int &indCount, int height, int width, bool rendered[6], int x, int y, int z);
	void buildTerrain();
	Cube cubeAt(int x, int y, int z);
	void deleteCube(int x, int y, int z);
	void addCube(BlockType type, int x, int y, int z);
	void getBufferArray_1face(std::vector<float> &verts, std::vector<unsigned int> &inds, int &indCount, BlockType type, int face, int height, int width, float x, float y, float z);

private:
	// "3-d" array of cubes
	Cube * cubes;  
	Shader * m_shader;
	bool needsRebuild;

};


#endif 