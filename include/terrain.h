#ifndef TERRAIN_H
#define TERRAIN_H

#include <thread>
#include "pugixml.hpp"

#include "anl.h"
#include "implicitxml.h"
#include <irrlicht.h>
#include "marchingcubes.h"
#include "boost/multi_array.hpp"
#include "boost/array.hpp"
#include "brush.h"
#include "engine.h"

static irr::core::vector3d<int> dimensions(16,16,16);

typedef boost::multi_array<float, 3> Float3Array;
typedef boost::multi_array<unsigned, 3> Unsigned3Array;
typedef boost::multi_array<int, 2> Int2Array;

enum EChunkStatus {
    EMPTY,      //This node has not yet been assigned data
    FILLING,    //Currently assigning data to this node
    FILLED,     //This node needs to be meshed
    DIRTY,      //This node needs to be remeshed
    MESHING,    //This node is currently meshing
    CLEAN       //This node doesn't need
};

struct VoxelData
{
    irr::core::vector3d<int> Position;
    float Value;
    unsigned Material;
};

struct VoxelReference
{
public:
    irr::core::vector3d<int> Chunk;
    irr::core::vector3d<unsigned> Position;
    irr::core::vector3d<int> Dimensions;

    VoxelReference(irr::core::vector3d<int> position, irr::core::vector3d<int> dimensions)
    {
        std::div_t x_ref = std::div(position.X, dimensions.X);
        std::div_t y_ref = std::div(position.Y, dimensions.Y);
        std::div_t z_ref = std::div(position.Z, dimensions.Z);



        Chunk = irr::core::vector3d<int>(x_ref.quot, y_ref.quot, z_ref.quot);

        Position = irr::core::vector3d<unsigned>(
            (x_ref.rem > 0)? x_ref.rem : dimensions.X + x_ref.rem, 
            (y_ref.rem > 0)? y_ref.rem : dimensions.Y + y_ref.rem, 
            (z_ref.rem > 0)? z_ref.rem : dimensions.Z + z_ref.rem
        );

        //printf("Position (%i,%i,%i)\n", position.X, position.Y, position.Z);
        //printf("Chunk (%i,%i,%i)\n", Chunk.X, Chunk.Y, Chunk.Z);
        //printf("Offset (%i,%i,%i)\n", Position.X, Position.Y, Position.Z);

        Dimensions = dimensions;
    }

    VoxelReference(irr::core::vector3d<int> chunk, irr::core::vector3d<unsigned> position, irr::core::vector3d<int> dimensions)
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

class TerrainChunk
{
public:
    Float3Array * values; //Provides contents for each cell
    Unsigned3Array * materials; //Provites material for each cell
    Int2Array * heights; //Provides highest point in X,Z plane, returns null for empty colmun.

    irr::core::vector3d<int> * localPoint;
    int status;

    bool empty; //No visible cubes;
    bool obstruct; //At least one horizontal plane is filled

    //irr::scene::SMesh * Mesh;
    irr::scene::SMeshBuffer * buffer;
    irr::scene::SMeshBuffer * tempBuffer;

    TerrainChunk(){
        values = new Float3Array(boost::extents[dimensions.X+1][dimensions.Y+1][dimensions.Z+1]);
        materials = new Unsigned3Array(boost::extents[dimensions.X+1][dimensions.Y+1][dimensions.Z+1]);
        heights = new Int2Array(boost::extents[dimensions.X+1][dimensions.Z+1]);
        localPoint = new irr::core::vector3d<int>();
        buffer = new irr::scene::SMeshBuffer();

        status = EMPTY;

        empty = true;
        obstruct = false;
    };
    void Initialize(irr::core::vector3d<int> dimensions, irr::core::vector3d<int> position);

    double GetValue(irr::core::vector3d<unsigned> position);
    bool GetFilled(irr::core::vector3d<unsigned> position);
    int GetHeight(irr::core::vector2d<unsigned> position);

    void UpdateVoxel(irr::core::vector3d<unsigned> position, float value, int material);
    void FillChunk(anl::CImplicitXML & noiseTree);
    void MeshChunk();
};


class ScalarTerrain
{
private:
	double value;


    static const int MAXTHREADS = 10;

public:
    anl::CImplicitXML noiseTree;
    std::map<irr::core::vector3d<int>, TerrainChunk> worldMap;
    std::map<irr::core::vector3d<int>, TerrainChunk>::iterator worldMap_iterator;
    irr::io::path vsFileName;
    irr::io::path psFileName;
    irr::scene::SMesh Mesh;
    irr::video::SMaterial Material;
    irr::s32 terrainMaterial;

    int threads;
    int meshThreads;
    int fillThreads;

	ScalarTerrain();

	~ScalarTerrain(){};

    void Init();
	void setupAccidentalNoise();
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
	void generateNavMesh();
	void getMesh(/*frustum*/);
    void generateMesh(const irr::scene::SViewFrustum * Frustum);
    void FillBackground(irr::core::vector3d<int> tl);
    void MeshBackground(irr::core::vector3d<int> tl);

    void UpdateVoxel(VoxelData & vd);

    int GetAltitude(const irr::core::vector3d<int> & Position);
    int GetAltitude(const irr::core::vector3df & Position);
    bool GetFilled(const irr::core::vector3d<int> & Position);
    bool GetFilled(const irr::core::vector3df & Position);

    bool BlockFilled(irr::core::vector3df);
};

#endif