#ifndef CUBE_H_
#define CUBE_H_


class Cube 
{
public:
	enum BlockType {
		BlockType_Default = 0,
		BlockType_Grass,
		BlockType_Dirt,
		BlockType_Water,
		BlockType_Stone,
		BlockType_Wood,
		BlockType_Sand,
		BlockType_NumTypes,
	};
	

	Cube();
	Cube(BlockType type, bool active);
	~Cube();
	bool IsActive();
	void setActive(bool active);

private:
	BlockType type;
	bool isActive;
};

void getBufferArray(float* buff, unsigned int* indices, bool render[6]);

#endif 