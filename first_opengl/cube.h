#ifndef CUBE_H_
#define CUBE_H_


class Cube 
{
public:
	int xpos;
	int ypos;
	int zpos;
	int type;
	bool renderedFaces[];  // front back left right bottom top

	Cube(int xpos, int ypos, int zpos, int type);
	/*
	~Cube();
	Cube(const Cube& c);
	*/
	// creates buffer array of vertecies and textures and indicies array
	// render: array of bools indicating if the face is rendered
	//void getBufferArray(float* buff, unsigned int* indices, bool render[6], int numFaces);
	int numRenderedFaces();


};

void getBufferArray(float* buff, unsigned int* indices, bool render[6]);

#endif CUBE_H_
