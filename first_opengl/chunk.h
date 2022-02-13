#ifndef CHUNK_H_
#define CHUNK_H_

#include "cube.h"
class Chunk
{
public:
	// startX,Z is smallest coordinate
	// so chunk spans (startX -> startX + 16), etc
	int startX;
	int startZ;	
	unsigned int numCubes;
	Chunk(int xpos, int zpos);
	~Chunk();
	void getCubeFaces(int x, int y, int z, bool(&render)[6]);
private:
	// "3-d" array of cubes
	int * cubes;  // 1 if present, 0 if not

};


#endif CHUNK_H_
