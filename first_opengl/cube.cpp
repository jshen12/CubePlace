#include "cube.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "config.h"

Cube::Cube()
{
    isActive = false;
    type = BlockType::BlockType_Default;

}

Cube::Cube(BlockType t, bool active)
{
    isActive = active;
    type = t;
}


Cube::~Cube()
{
   
}

bool Cube::IsActive()
{
    return isActive;
}

void Cube::setActive(bool active)
{
    isActive = active;
}

BlockType Cube::getType()
{
    return type;
}

void Cube::setType(BlockType t)
{
    type = t;
}





