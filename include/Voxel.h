#ifndef VOXEL_H
#define VOXEL_H

#include <thread>
#include "pugixml.hpp"

#include "anl.h"
#include "implicitxml.h"
#include <irrlicht.h>
#include "boost/multi_array.hpp"
#include "boost/array.hpp"
#include "brush.h"
#include "engine.h"

static irr::core::vector3d<unsigned> dimensions(16,16,16);
static float isolevel = 0.5;

typedef boost::multi_array<float, 3> Float3Array;
typedef boost::multi_array<bool, 3> Bool3Array;
typedef boost::multi_array<unsigned, 3> Unsigned3Array;
typedef boost::multi_array<int, 2> Int2Array;

enum EChunkStatus {
    EMPTY,      //(0) This node has not yet been assigned data
    FILLING,    //(1) Currently assigning data to this node
    FILLED,     //(2) This node needs to be meshed
    DIRTY,      //(3) This node needs to be remeshed
    MESHING,    //(4) This node is currently meshing
    CLEAN       //(5) This node doesn't need
};

struct VoxelData
{
    irr::core::vector3d<int> Position;
    float Value;
    unsigned Material;

    VoxelData(irr::core::vector3d<int> _position, float _value, unsigned _material)
    {
        Position = _position;
        Value = _value;
        Material = _material;
    }
};

struct VoxelReference
{
public:
    irr::core::vector3d<int> Chunk;
    irr::core::vector3d<int> Position;
    irr::core::vector3d<unsigned> Dimensions;

    VoxelReference(irr::core::vector3d<int> position)
    {
        std::div_t x_ref = std::div(position.X, (int)dimensions.X);
        std::div_t y_ref = std::div(position.Y, (int)dimensions.Y);
        std::div_t z_ref = std::div(position.Z, (int)dimensions.Z);

        Chunk = irr::core::vector3d<int>(
            (position.X < 0 && x_ref.rem != 0)? x_ref.quot - 1 : x_ref.quot, 
            (position.Y < 0 && y_ref.rem != 0)? y_ref.quot - 1 : y_ref.quot, 
            (position.Z < 0 && z_ref.rem != 0)? z_ref.quot - 1 : z_ref.quot
        );

        Position = irr::core::vector3d<int>(
            (position.X < 0 && x_ref.rem != 0)? (int)dimensions.X + x_ref.rem : x_ref.rem, 
            (position.Y < 0 && y_ref.rem != 0)? (int)dimensions.Y + y_ref.rem : y_ref.rem, 
            (position.Z < 0 && z_ref.rem != 0)? (int)dimensions.Z + z_ref.rem : z_ref.rem
        );

        Dimensions = dimensions;

        //printf("input(%i,%i,%i)", position.X, position.Y, position.Z);

        //printf(" quot(%i,%i,%i)", x_ref.quot, y_ref.quot, z_ref.quot);
        //printf(" rem(%i,%i,%i)", x_ref.rem, y_ref.rem, z_ref.rem);

        //printf(" output c(%i,%i,%i)p(%i,%i,%i)\n",Chunk.X, Chunk.Y, Chunk.Z,Position.X, Position.Y, Position.Z);

        assert(Position.X < (int)Dimensions.X);
        assert(Position.Y < (int)Dimensions.Y);
        assert(Position.Z < (int)Dimensions.Z);
    }

    VoxelReference(irr::core::vector3d<int> chunk, irr::core::vector3d<int> position)
    {
        Chunk = chunk;
        Position = position;
        Dimensions = dimensions;
    }

    irr::core::vector3d<int> GetRealPosition(irr::core::vector3d<int> Size)
    {
        irr::core::vector3d<int> position((Chunk.X * Dimensions.X) + Position.X, (Chunk.Y * Dimensions.Y) + Position.Y, (Chunk.Z * Dimensions.Z) + Position.Z);

        return position;
    }
};

class VoxelBrush
{
public:
    std::vector<VoxelBrush> * vVoxels;
};



class VoxelSceneNode;

class VoxelChunk
{
public:
    Float3Array * values; //Provides contents for each cell
    Unsigned3Array * materials; //Provites material for each cell
    Int2Array * heights; //Provides highest point in X,Z plane, returns null for empty colmun.
    Bool3Array * filled;

    VoxelSceneNode * parent;

    irr::core::vector3d<int> localPoint;
    irr::core::vector3d<int> chunkPos;
    int status;

    bool empty; //No visible cubes;
    bool obstruct; //At least one horizontal plane is filled

    VoxelChunk()
    {
        values = new Float3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        materials = new Unsigned3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        filled = new Bool3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        heights = new Int2Array(boost::extents[dimensions.X][dimensions.Z]);

        status = EMPTY;

        empty = false;
        obstruct = false;
    };
    void Initialize(VoxelSceneNode * parent, irr::core::vector3d<int>);

    float GetValue(irr::core::vector3d<int> position);
    void SetValue(irr::core::vector3d<int> position, float value);

    int GetMaterial(irr::core::vector3d<int> position);
    void SetMaterial(irr::core::vector3d<int> position, int material);

    bool GetFilled(irr::core::vector3d<int> position);
    int GetHeight(irr::core::vector2d<int> position);

    void UpdateVoxel(irr::core::vector3d<int> position, float value, int material, bool subtract);
    void FillChunk(anl::CImplicitXML & noiseTree);
    void MeshChunk();
};

/*
* This should be extended to implement various voxel rendering techniques.
*/
class ChunkMesh
{
public:
    int status;

    bool Meshed;

    irr::scene::SMeshBuffer * buffer;
    irr::scene::SMeshBuffer * tempBuffer;
    irr::core::vector3d<int> localPoint;
    irr::core::vector3d<int> chunkPos;

    VoxelSceneNode * parent;

    Unsigned3Array * generatedPoints;

    ChunkMesh(){};


    void Initialize(VoxelSceneNode * parent, irr::core::vector3d<int> tl);
    void GenerateMesh();
    void NaiveNormals();
    void GenerateSurface(irr::core::vector3d<int> renderBlock, float Values[8], int Materials[8]);
};
#endif