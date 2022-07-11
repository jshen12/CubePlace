#ifndef CUBE_H_
#define CUBE_H_


enum class BlockType {
	BlockType_Air = 0,
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
	Cube(BlockType type);
	~Cube();
	bool IsActive();
	BlockType getType();
	void setType(BlockType t);
private:
	BlockType type;
};


#endif 