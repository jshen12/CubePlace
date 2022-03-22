#ifndef CUBE_H_
#define CUBE_H_

enum class BlockType {
	BlockType_Default = 0,
	BlockType_Grass,
	BlockType_Dirt,
	BlockType_Water,
	BlockType_Stone,
	BlockType_Wood,
	BlockType_Sand,
	BlockType_NumTypes,
};

class Cube 
{
public:
	
	Cube();
	Cube(BlockType type, bool active);
	~Cube();
	bool IsActive();
	void setActive(bool active);
	BlockType getType();
	void setType(BlockType t);
private:
	BlockType type;
	bool isActive;
};

void getBufferArray(float* buff, unsigned int* indices, bool render[]);

#endif 