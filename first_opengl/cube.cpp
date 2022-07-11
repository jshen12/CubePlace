#include "cube.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "config.h"

Cube::Cube()
{
    type = BlockType::BlockType_Air;

}

Cube::Cube(BlockType t)
{
    type = t;
}


Cube::~Cube()
{
   
}

bool Cube::IsActive()
{
    return (type!=BlockType::BlockType_Air);
}


BlockType Cube::getType()
{
    return type;
}

void Cube::setType(BlockType t)
{
    type = t;
}





