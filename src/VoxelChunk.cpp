#include "Voxel.h"
#include "anl.h"

int VoxelChunk::GetHeight(irr::core::vector2d<unsigned> Position)
{
    if(status != EMPTY && status != FILLING)
    {
        if(!(*heights)[Position.X][Position.Y])
        {
            return -1;
        }
        else
        {
            return (*heights)[Position.X][Position.Y];
        }
    }
    else
    {
        return -1;
    }
}

bool VoxelChunk::GetFilled(irr::core::vector3d<unsigned> Position)
{
    if(status != EMPTY && status != FILLING)
    {
        if(!(*values)[Position.X][Position.X][Position.X])
        {
            return false;
        }
        else
        {
            return (*values)[Position.X][Position.X][Position.X] > 0.5;
        }
    }
    else
    {
        return false;
    }
}

void VoxelChunk::Initialize(irr::core::vector3d<unsigned> dimensions, irr::core::vector3d<int> position)
{
    localPoint = irr::core::vector3d<int>(position.X,position.Y,position.Z);
}

float VoxelChunk::GetValue(irr::core::vector3d<unsigned> Position)
{
    if(status != EMPTY && status != FILLING)
    {
        return (*values)[Position.X][Position.Y][Position.Z];
    }
    else
    {
        return -15.f;
    }
}

int VoxelChunk::GetMaterial(irr::core::vector3d<unsigned> Position)
{
    if(status != EMPTY && status != FILLING)
    {
        return (*values)[Position.X][Position.Y][Position.Z];
    }
    else
    {
        return 0;
    }
}

void VoxelChunk::UpdateVoxel(irr::core::vector3d<unsigned> position, float value, int material, bool subtract)
{
    if(subtract)
    {
        if((*values)[position.X][position.Y][position.Z] > value)
        {
            (*values)[position.X][position.Y][position.Z] = value;
            (*materials)[position.X][position.Y][position.Z] = material;
            status = DIRTY;
        }
    }
    else
    {
        if((*values)[position.X][position.Y][position.Z] < value)
        {
            (*values)[position.X][position.Y][position.Z] = value;
            (*materials)[position.X][position.Y][position.Z] = material;
            status = DIRTY;
        }
    }

    
}

/**
Using noise tree, populates an array with values
**/
void VoxelChunk::FillChunk(anl::CImplicitXML & noiseTree) {
    double value;
    int xPos = localPoint.X * (int)dimensions.X;
    int yPos = localPoint.Y * (int)dimensions.Y;
    int zPos = localPoint.Z * (int)dimensions.Z;

    bool solid;

    //TODO: This method could essentially generate a list of meshable voxels (see improvements for marchingcubes.cpp)

    try {
        for(int y = dimensions.Y -1; y >= 0; y--) { //Trace DOWN y axis, first blocks found will be highest.
            solid=true; //fresh layer, assume solid
            for(unsigned x = 0; x < dimensions.X; x++) {
                for(unsigned z = 0; z < dimensions.Z; z++) {
                    
                    value = noiseTree.get(
                        (double) ((int)x + xPos) / 32.f, // (double) x_chunk, 
                        (double) (y + yPos) / 32.f, // (double) y_chunk, 
                        (double) ((int)z + zPos) / 32.f // (double) z_chunk
                    );

                    //printf("(%i,%i,%i)+(%i,%i,%i)=(%i,%i,%i)", x, y, z, xPos, yPos, zPos, x+xPos, y+yPos, z+zPos);

                    //printf("%f\n", value);

                    if(value > isolevel) {
                        empty = false; //a value has been found, block must be meshed

                        //if there is no highest block in this column, set this column's highest value to y.
                        //This effectively generates a height map, and vertical occlusion map for each chunk.
                        if(!(*heights)[x][z])
                        {
                            (*heights)[x][z] = y; 
                        }
                    } else {
                        solid = false; //a hole has been found, this layer is not solid
                    }

                    (*values)[x][y][z] = value;
                    (*materials)[x][y][z] = 0;
                }
            }
            if(solid) obstruct=true; //if an entire layer is solid, chunk obstructs vertically
        }
        //Chunk is clean, allow rendering.
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}