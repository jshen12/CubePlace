#include "world.h"
#include <vector>
#include <omp.h>
#include <chrono>

#define coordToArray(x, y, z) (x * yChunk * zChunk + y * zChunk + z)


World::World(Shader& shader, int h, int w, GLuint vert_arr, GLuint vert_buff, GLuint ele_buff)
{
	m_shader = &shader;
	height = h;
	width = w;
	vertex_array = vert_arr;
	vertex_buffer = vert_buff;
	element_buffer = ele_buff;
	threadstatus = ThreadStatus::Idle;
	needsRebuild = false;
	indCount = 0;
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
	new_indices = {};
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

void World::rebuildIndices(int numInd)
{
	if (numInd == new_indices.size())
		return;
	else if (numInd > new_indices.size()) {   // grow total_indices
		new_indices.reserve(numInd);
		while (numInd > new_indices.size()) {
			new_indices.push_back(indCount);
			new_indices.push_back(indCount + 1);
			new_indices.push_back(indCount + 2);
			new_indices.push_back(indCount + 2);
			new_indices.push_back(indCount + 3);
			new_indices.push_back(indCount);

			indCount += 4;
		}
	}
	else {                                      // shrink total_indices
		new_indices.resize(numInd);
	}
}

void World::breakBlock(glm::vec3 posVector, glm::vec3 sightVector)
{
	// iteratively step through raycast, checking if block is present
	// to find face, check which coordinate plane was crossed (1.0 x, 2.0 y, etc.) Use math.floor()
	glm::vec3 currentPoint = glm::vec3(posVector.x, posVector.y, posVector.z);
	int x, y, z;
	double distTraveled = 0;
	double step = STEPPING_DISTANCE;
	while (distTraveled < MAX_SELECTION_DISTANCE)
	{
		currentPoint.x += sightVector.x * STEPPING_DISTANCE;
		currentPoint.y += sightVector.y * STEPPING_DISTANCE;
		currentPoint.z += sightVector.z * STEPPING_DISTANCE;
		x = static_cast<int>(currentPoint.x);
		y = static_cast<int>(currentPoint.y);
		z = static_cast<int>(currentPoint.z);
		auto ch = chunkMap.find(std::pair<int, int>(std::floor(x / xChunk) * xChunk, std::floor( z / zChunk) * zChunk));
		if (ch == chunkMap.end())
			return;
		if (ch->second->cubeAt(x % xChunk, y, z % zChunk).getType() != BlockType::BlockType_Air) {
			ch->second->cubeAt(x % xChunk, y, z % zChunk).setType(BlockType::BlockType_Air);
			ch->second->setRebuildStatus(true);
			// TODO: add chunk borders too 
			printf("%d %d %d", x, y, z);

			needsRebuild = true;
			return;
		}
		distTraveled += STEPPING_DISTANCE;
	}
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

// takes copy of world (or list of changes?), returns (or sets) VBO: maintain queue of threads
// share memory? (but what if memory changes while viewing) or send copy? (too large)
// then communicate with main thread indicating that they are done (unless shared memory)
// idea: instead of shared memory, use copy of data from each chunk class???
void World::UpdateVBO()
{
	
	auto t1 = std::chrono::high_resolution_clock::now();
	threadstatus = ThreadStatus::Working;
	unsigned int numInds = 0;
	for (auto ch : chunkMap) {
		if (ch.second->doesNeedRebuild()) {
			ch.second->clearVertices();
			Cube currCube;
			// for every cube
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
							if (facesCount > 0)     // dont bother if no faces are rendered anyways
								ch.second->renderFaces(height, width, rendered, x, y, z);
							
						}
					}
				}
			}
			ch.second->setRebuildStatus(false);
		}
		numInds += ch.second->numIndices;
	}
	
	rebuildIndices(numInds);
	threadstatus = ThreadStatus::Done;

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms_double = t2 - t1;
	std::cout << "Buffer built in :" << ms_double.count() << "ms\n";
}

void World::renderChunks(float currX, float currZ)
{
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
	for (int x = -(MAX_CHUNK_DISTANCE - 1) * xChunk; x < (MAX_CHUNK_DISTANCE) * xChunk; x += xChunk) {
		for (int z = -(MAX_CHUNK_DISTANCE - 1) * zChunk; z < (MAX_CHUNK_DISTANCE) * zChunk ; z += zChunk) {
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
	if (needsRebuild && (threadstatus == ThreadStatus::Idle))
	{
		for (auto& t : threadstack)
			t.join();
		threadstack.clear();
		threadstack.push_back(std::thread(&World::UpdateVBO, this));  
		
	}
	else if (needsRebuild && (threadstatus == ThreadStatus::Done)) {
		auto t1 = std::chrono::high_resolution_clock::now();
		for (auto& t : threadstack) 
			t.join();
		threadstack.clear();
		clearVectors();
		for (auto ch : chunkMap) {
			std::vector<float> chunk_verts = ch.second->getVertices();
			total_vertices.insert(total_vertices.end(), chunk_verts.begin(), chunk_verts.end());
		}
		total_indices = new_indices;
		threadstatus = ThreadStatus::Idle;
		needsRebuild = false;

		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> ms_double = t2 - t1;
		std::cout << "Buffer copied in :" << ms_double.count() << "ms\n";
	}
	drawMesh();

}
