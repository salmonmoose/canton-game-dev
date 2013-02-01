#include "Voxel.h"
#include "anl.h"

int VoxelChunk::GetHeight(irr::core::vector2d<int> Position)
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

bool VoxelChunk::GetFilled(irr::core::vector3d<int> Position)
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

void VoxelChunk::Initialize(VoxelSceneNode * _parent, irr::core::vector3d<int> position)
{
    parent = _parent;

    chunkPos = position;

    localPoint = irr::core::vector3d<int>(
        position.X * (int)dimensions.X,
        position.Y * (int)dimensions.Y,
        position.Z * (int)dimensions.Z
    );

#ifdef _VOXEL_DEBUG_
    printf("Initialized at (%i,%i,%i)\n",
        localPoint.X,
        localPoint.Y,
        localPoint.Z
        );
#endif
}

float VoxelChunk::GetValue(irr::core::vector3d<int> Position)
{
#ifdef _VOXEL_DEBUG_
    if(Position.X == 0 && Position.Y == 0 && Position.Z == 0)
        printf("Getting (%i,%i,%i) of %f\n", 
            localPoint.X + Position.X, 
            localPoint.Y + Position.Y, 
            localPoint.Z + Position.Z, 
            (*values)[Position.X][Position.Y][Position.Z]  
        );
#endif
    return (*values)[Position.X][Position.Y][Position.Z];
}

void VoxelChunk::SetValue(irr::core::vector3d<int> Position, float Value)
{
#ifdef _VOXEL_DEBUG_
    if(Position.X == 0 && Position.Y == 0 && Position.Z == 0)
        printf("Setting (%i,%i,%i) of %f\n", 
            localPoint.X + Position.X, 
            localPoint.Y + Position.Y, 
            localPoint.Z + Position.Z, 
            Value
        );
#endif
    (*values)[Position.X][Position.Y][Position.Z] = Value;
}

int VoxelChunk::GetMaterial(irr::core::vector3d<int> Position)
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

void VoxelChunk::UpdateVoxel(irr::core::vector3d<int> position, float value, int material, bool subtract)
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
            empty = false;
        }
    }

    
}

/**
Using noise tree, populates an array with values
**/
void VoxelChunk::FillChunk(anl::CImplicitXML & noiseTree) {
    double value;

    empty=false;

    bool solid;

    //TODO: This method could essentially generate a list of meshable voxels (see improvements for marchingcubes.cpp)
    try {
        for(int y = (int)dimensions.Y -1; y >= 0; y--) { //Trace DOWN y axis, first blocks found will be highest.
            solid=true; //fresh layer, assume solid
            for(int x = 0; x < (int)dimensions.X; x++) {
                for(int z = 0; z < (int)dimensions.Z; z++) {

                    value = noiseTree.get(
                        (double) (x + localPoint.X) / 32.f, // (double) x_chunk, 
                        (double) (y + localPoint.Y) / 32.f, // (double) y_chunk, 
                        (double) (z + localPoint.Z) / 32.f // (double) z_chunk
                    );

                    if(value > isolevel) {
                        empty = false; //a value has been found, block must be meshed

                        if(!(*heights)[x][z])
                        {
                            (*heights)[x][z] = y; 
                        }
                    } else {
                        solid = false; //a hole has been found, this layer is not solid
                    }

                    parent->SetValue(irr::core::vector3d<int>(x + localPoint.X, y + localPoint.Y, z + localPoint.Z), value);

                    //float parentValue = parent->GetValue(irr::core::vector3d<int>(x + localPoint.X, y + localPoint.Y, z + localPoint.Z));
                    float parentValue = GetValue(irr::core::vector3d<int>(x, y, z));

                    (*materials)[x][y][z] = 0;
                }
            }
            if(solid) obstruct = true; //if an entire layer is solid, chunk obstructs vertically
        }
        //Chunk is clean, allow rendering.
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}