
#include "terrain.h"
#include "anl.h"

int TerrainChunk::GetHeight(irr::core::vector2d<unsigned> Position)
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

bool TerrainChunk::GetFilled(irr::core::vector3d<unsigned> Position)
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

void TerrainChunk::Initialize(irr::core::vector3d<int> dimensions, irr::core::vector3d<int> position)
{
    buffer->setBoundingBox(irr::core::aabbox3df(
        (dimensions.X * position.X), 
        (dimensions.Y * position.Y), 
        (dimensions.Z * position.Z),
        (dimensions.X * position.X + dimensions.X), 
        (dimensions.Y * position.Y + dimensions.Y), 
        (dimensions.Z * position.Z + dimensions.Z)
    ));
    localPoint = new irr::core::vector3d<int>(position.X,position.Y,position.Z);
}

double TerrainChunk::GetValue(irr::core::vector3d<unsigned> Position)
{
    if(status != EMPTY && status != FILLING)
    {
        return (*values)[Position.X][Position.X][Position.X];
    }
    else
    {
        return 0.f;
    }
}

void TerrainChunk::MeshChunk()
{
    tempBuffer = new irr::scene::SMeshBuffer();
    generateIsoSurface(* tempBuffer, * values, * materials, localPoint->X * dimensions.X, localPoint->Y * dimensions.Y, localPoint->Z * dimensions.Z);
    buffer = tempBuffer;
}

void TerrainChunk::UpdateVoxel(irr::core::vector3d<unsigned> position, float value, int material)
{
    (*values)[position.X][position.Y][position.Z] = value;
    (*materials)[position.X][position.Y][position.Z] = material;

    status = DIRTY;
}

/**
Using noise tree, populates an array with values
**/
void TerrainChunk::FillChunk(anl::CImplicitXML & noiseTree) {
    double value;
    double xPos = localPoint->X * dimensions.X;
    double yPos = localPoint->Y * dimensions.Y;
    double zPos = localPoint->Z * dimensions.Z;

    bool solid;

    //TODO: This method could essentially generate a list of meshable voxels (see improvements for marchingcubes.cpp)

    try {
        for(int y = dimensions.Y; y >= 0; y--) { //Trace DOWN y axis, first blocks found will be highest.
            solid=true; //fresh layer, assume solid
            for(int x = 0; x <= dimensions.X; x++) {
                for(int z = 0; z <= dimensions.Z; z++) {
                    
                    value = noiseTree.get(
                        (double) (x + xPos) / 32.f, // (double) x_chunk, 
                        (double) (y + yPos) / 32.f, // (double) y_chunk, 
                        (double) (z + zPos) / 32.f // (double) z_chunk
                    );

                    if(value > 0.5) {
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