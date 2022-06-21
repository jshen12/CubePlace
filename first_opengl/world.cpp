#include "world.h"
#include <vector>

#define coordToArray(x, y, z) (x * yChunk * zChunk + y * zChunk + z)


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
	for (auto ch : chunkMap) {
		delete ch.second;
		chunkMap.erase(std::make_pair(ch.second->startX, ch.second->startZ));
	}
}

void World::initWorld()
{
	for (int x = -(MAX_CHUNK_DISTANCE); x < MAX_CHUNK_DISTANCE + 1; x++) {
		for (int z = -(MAX_CHUNK_DISTANCE); z < MAX_CHUNK_DISTANCE + 1; z++) {
			Chunk *ch = new Chunk(x * xChunk, z * zChunk, *m_shader);
			chunkMap [std::make_pair(x * xChunk, z * zChunk)] = ch;
		}
	}
	total_vertices = {};
	total_indices = {};
	indCount = 0;
}

void World::buildWorld()
{
	for (auto ch : chunkMap) {
		ch.second->buildTerrain();
	}
}

void World::clearVectors()
{
	total_vertices.clear();
	total_indices.clear();
	indCount = 0;
}

void World::breakBlock(glm::vec3 posVector, glm::vec3 sightVector)
{
	// iteratively step through raycast, checking if block is present
	// to find face, check which coordinate plane was crossed (1.0 x, 2.0 y, etc.) Use math.floor()
	glm::vec3 currentPoint = glm::vec3(posVector.x, posVector.y, posVector.z);
	double distTraveled = 0;
	while (distTraveled < MAX_SELECTION_DISTANCE)
	{
		distTraveled += STEPPING_DISTANCE;
	}
	

}

void World::buildIndicesList()
{
	/*
	for (auto ch : chunkMap) {
		for (int i = 0; i < ch.second->numFaces; i++) {
			total_indices.push_back(indCount);
			total_indices.push_back(indCount + 1);
			total_indices.push_back(indCount + 2);
			total_indices.push_back(indCount + 2);
			total_indices.push_back(indCount + 3);
			total_indices.push_back(indCount);

			indCount += 4;
		}
	}
	*/
}

void World::calculateFaces(int x, int y, int z, Chunk &currChunk, bool rendered[])
{
	
	// top
	rendered[5] = !(y < (yChunk - 1) && currChunk.cubeAt(x, y + 1, z).IsActive());
	// bottom
	rendered[4] = !(y > 0 && currChunk.cubeAt(x, y - 1, z).IsActive());
	// right
	if (x == xChunk - 1) {
		std::pair<int, int> adjPair{ currChunk.startX + xChunk, currChunk.startZ };
		auto adjChunk = chunkMap.find(adjPair);
		if (adjChunk != chunkMap.end())  // found !
			rendered[3] = !adjChunk->second->cubeAt(0, y, z).IsActive();
		else
			rendered[3] = true; // edge, keep rendered
	}
	else
		rendered[3] = !currChunk.cubeAt(x + 1, y, z).IsActive();
	// left
	if (x == 0) {
		std::pair<int, int> adjPair{ currChunk.startX - xChunk, currChunk.startZ };
		auto adjChunk = chunkMap.find(adjPair);
		if (adjChunk != chunkMap.end())
			rendered[2] = !adjChunk->second->cubeAt(xChunk - 1, y, z).IsActive();
		else
			rendered[2] = true;
	}
	else
		rendered[2] = !currChunk.cubeAt(x - 1, y, z).IsActive();
	// back
	if (z == zChunk - 1) {
		std::pair<int, int> adjPair{ currChunk.startX, currChunk.startZ + zChunk };
		auto adjChunk = chunkMap.find(adjPair);
		if (adjChunk != chunkMap.end())  // found !
			rendered[1] = !adjChunk->second->cubeAt(x, y, 0).IsActive();
		else
			rendered[1] = true; // edge, keep rendered
	}
	else
		rendered[1] = !currChunk.cubeAt(x, y, z + 1).IsActive();
	// front
	if (z == 0) {

		std::pair<int, int> adjPair{ currChunk.startX, currChunk.startZ - zChunk };
		auto adjChunk = chunkMap.find(adjPair);
		if (adjChunk != chunkMap.end())  // found !
			rendered[0] = !adjChunk->second->cubeAt(x, y, zChunk - 1).IsActive();
			
		else
			rendered[0] = true; // edge, keep rendered
	}
	else
		rendered[0] = !currChunk.cubeAt(x, y, z - 1).IsActive();
}

void World::drawMesh()
{
	drawBufferData(vertex_array, vertex_buffer, element_buffer, &total_vertices[0], &total_indices[0],
		sizeof(float) * total_vertices.size(), sizeof(unsigned int) * total_indices.size(), total_indices.size());
}

void World::renderChunks(float currX, float currZ)
{
	bool needsRebuild = false;
	// delete far away chunks (and set rebuild status)
	for (auto ch : chunkMap) {
		if (abs(ch.second->startX - currX) > MAX_CHUNK_DISTANCE * xChunk || abs(ch.second->startZ - currZ) > MAX_CHUNK_DISTANCE * zChunk) {
			delete ch.second;
			chunkMap.erase(std::make_pair(ch.second->startX, ch.second->startZ));
			needsRebuild = true;
			// set rebuild status for adjecent chunks, if present
			auto adjChunk = chunkMap.find(std::make_pair(ch.second->startX - xChunk, ch.second->startZ));   // south
			if (adjChunk != chunkMap.end())
				adjChunk->second->setRebuildStatus(true);
			adjChunk = chunkMap.find(std::make_pair(ch.second->startX + xChunk, ch.second->startZ));   // north
			if (adjChunk != chunkMap.end())
				adjChunk->second->setRebuildStatus(true);
			adjChunk = chunkMap.find(std::make_pair(ch.second->startX, ch.second->startZ - zChunk));   // east
			if (adjChunk != chunkMap.end())
				adjChunk->second->setRebuildStatus(true);
			adjChunk = chunkMap.find(std::make_pair(ch.second->startX, ch.second->startZ + zChunk));   // west
			if (adjChunk != chunkMap.end())
				adjChunk->second->setRebuildStatus(true);
		}
	}
	
	
	// add new chunks (and set rebuild status)
	int nearestX;
	int nearestZ;
	for (int x = -(MAX_CHUNK_DISTANCE - 1) * xChunk; x < (MAX_CHUNK_DISTANCE - 1) * xChunk; x += xChunk) {
		for (int z = -(MAX_CHUNK_DISTANCE - 1) * zChunk; z < (MAX_CHUNK_DISTANCE - 1) * zChunk ; z += zChunk) {
			nearestX = std::floor(currX / xChunk) * xChunk + x;
			nearestZ = std::floor(currZ / zChunk) * zChunk + z;
			if (chunkMap.find(std::make_pair(nearestX, nearestZ)) == chunkMap.end()) {
				needsRebuild = true;
				Chunk* ch = new Chunk(nearestX, nearestZ, *m_shader);
				ch->buildTerrain();
				chunkMap[std::make_pair(nearestX, nearestZ)] = ch;
				
				auto adjChunk = chunkMap.find(std::make_pair(nearestX - xChunk, nearestZ));   // east
				if (adjChunk != chunkMap.end()) 
					adjChunk->second->setRebuildStatus(true);
				adjChunk = chunkMap.find(std::make_pair(nearestX + xChunk, nearestZ));   // west
				if (adjChunk != chunkMap.end())
					adjChunk->second->setRebuildStatus(true);
				adjChunk = chunkMap.find(std::make_pair(nearestX, nearestZ - zChunk));   // south
				if (adjChunk != chunkMap.end())
					adjChunk->second->setRebuildStatus(true);
				adjChunk = chunkMap.find(std::make_pair(nearestX, nearestZ + zChunk));   // north
				if (adjChunk != chunkMap.end())
					adjChunk->second->setRebuildStatus(true);
			}
		}
	}
	
	if (needsRebuild)
	{
		clearVectors();
		for (auto ch : chunkMap) {
			Cube currCube;
			// for every cube
			//ch.second->clearVectors();   // reset buffer
			for (int x = ch.second->startX; x < xChunk + ch.second->startX; x++) {
				for (int y = 0; y < yChunk; y++) {
					for (int z = ch.second->startZ; z < zChunk + ch.second->startZ; z++) {

						currCube = ch.second->cubeAt(x - ch.second->startX, y, z - ch.second->startZ);
						if (currCube.IsActive()) {

							bool rendered[6] = {};
							calculateFaces(x - ch.second->startX, y, z - ch.second->startZ, *ch.second, rendered);

							int facesCount = 0;
							for (int i = 0; i < 6; i++)
								facesCount += rendered[i];
							if (facesCount > 0) {    // dont bother if no faces are rendered anyways
								ch.second->renderFaces(total_vertices, total_indices, indCount, height, width, rendered, x, y, z);
							}

						}

					}
				}
			}
			ch.second->setRebuildStatus(false);
		}
		buildIndicesList();
	}
	drawMesh();
	/*
	// update chunks (if needed)
	for (auto ch : chunkMap) {
		Cube currCube;
        // for every cube
		if (ch.second->doesNeedRebuild()) {   // only after chunk update
			ch.second->clearVectors();   // reset buffer
			for (int x = ch.second->startX; x < xChunk + ch.second->startX; x++) {
				for (int y = 0; y < yChunk; y++) {
					for (int z = ch.second->startZ; z < zChunk + ch.second->startZ; z++) {

						currCube = ch.second->cubeAt(x - ch.second->startX, y, z - ch.second->startZ);
						if (currCube.IsActive()) {

							bool rendered[6] = {};
							calculateFaces(x - ch.second->startX, y, z - ch.second->startZ, *ch.second, rendered);

							int facesCount = 0;
							for (int i = 0; i < 6; i++)
								facesCount += rendered[i];
							if (facesCount > 0) {    // dont bother if no faces are rendered anyways
								ch.second->renderFaces(height, width, rendered, x, y, z);
							}
							
						}

					}
				}
			}
			ch.second->setRebuildStatus(false);
		}
		ch.second->drawMesh(vertex_array, vertex_buffer, element_buffer);

	}
	*/
}
