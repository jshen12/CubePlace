#include "chunk.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
Chunk::Chunk(int xpos, int zpos)
{
	cubes = new int[16 * 50 * 16];
	startX = xpos;
	startZ = zpos;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 50; y++) {
			for (int z = 0; z < 16; z++) {
				if (y < 25)
					cubes[x * 50 * 16 + y * 16 + z] = 1;
				else {
					cubes[x * 50 * 16 + y * 16 + z] = 1;
				}
			}
		}
	}
	numCubes = 25 * 16 * 16;
}

Chunk::~Chunk()
{
	delete[] cubes;
}


void Chunk::getCubeFaces(int x, int y, int z, bool (&render)[6])
{
	// bottom
	render[5] = !(y > 0 && cubes[x * 50 * 16 + (y - 1) * 16 + z]);
	// top
	render[4] = !(y < 49 && cubes[x * 50 * 16 + (y + 1) * 16 + z]);
	// right
	render[3] = !(x < 15 && cubes[(x+1) * 50 * 16 + y * 16 + z]);
	// left
	render[2] = !(x > 0 && cubes[(x-1) * 50 * 16 + y * 16 + z]);
	// back
	render[1] = !(z < 15 && cubes[x * 50 * 16 + y * 16 + (z + 1)]);
	// front
	render[0] = !(z > 0 && cubes[x * 50 * 16 + y * 16 + (z - 1)]);

}

