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

static irr::core::vector3d<unsigned> dimensions(16,16,16);

typedef boost::multi_array<float, 3> Float3Array;
typedef boost::multi_array<bool, 3> Bool3Array;
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
    irr::core::vector3d<unsigned> Position;
    irr::core::vector3d<unsigned> Dimensions;

    VoxelReference(irr::core::vector3d<int> position)
    {
        std::div_t x_ref = std::div(position.X, (int)dimensions.X);
        std::div_t y_ref = std::div(position.Y, (int)dimensions.Y);
        std::div_t z_ref = std::div(position.Z, (int)dimensions.Z);

        Chunk = irr::core::vector3d<int>(x_ref.quot, y_ref.quot, z_ref.quot);

        Position = irr::core::vector3d<unsigned>(
            (x_ref.rem < 0)? (int)dimensions.X + x_ref.rem -1 : x_ref.rem, 
            (y_ref.rem < 0)? (int)dimensions.Y + y_ref.rem -1 : y_ref.rem, 
            (z_ref.rem < 0)? (int)dimensions.Z + z_ref.rem -1 : z_ref.rem
        );

        Dimensions = dimensions;

        //printf("input(%i,%i,%i)", position.X, position.Y, position.Z);

        //printf(" quot(%i,%i,%i)", x_ref.quot, y_ref.quot, z_ref.quot);
        //printf(" rem(%i,%i,%i)", x_ref.rem, y_ref.rem, z_ref.rem);

        //printf(" output c(%i,%i,%i)p(%i,%i,%i)\n",Chunk.X, Chunk.Y, Chunk.Z,Position.X, Position.Y, Position.Z);

        assert(Position.X < Dimensions.X);
        assert(Position.Y < Dimensions.Y);
        assert(Position.Z < Dimensions.Z);
    }

    VoxelReference(irr::core::vector3d<int> chunk, irr::core::vector3d<unsigned> position, irr::core::vector3d<unsigned> dimensions)
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
    Bool3Array * filled;

    irr::core::vector3d<int> localPoint;
    int status;

    bool empty; //No visible cubes;
    bool obstruct; //At least one horizontal plane is filled

    //irr::scene::SMesh * Mesh;
    irr::scene::SMeshBuffer * buffer;
    irr::scene::SMeshBuffer * tempBuffer;

    TerrainChunk()
    {
        values = new Float3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        materials = new Unsigned3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        filled = new Bool3Array(boost::extents[dimensions.X][dimensions.Y][dimensions.Z]);
        heights = new Int2Array(boost::extents[dimensions.X][dimensions.Z]);
        buffer = new irr::scene::SMeshBuffer();

        status = EMPTY;

        empty = true;
        obstruct = false;
    };
    void Initialize(irr::core::vector3d<unsigned> dimensions, irr::core::vector3d<int> position);

    float GetValue(irr::core::vector3d<unsigned> position);
    int GetMaterial(irr::core::vector3d<unsigned> position);
    bool GetFilled(irr::core::vector3d<unsigned> position);
    int GetHeight(irr::core::vector2d<unsigned> position);

    void UpdateVoxel(irr::core::vector3d<unsigned> position, float value, int material);
    void FillChunk(anl::CImplicitXML & noiseTree);
    void MeshChunk();
};

class TerrainMesh;

class ScalarTerrain
{
private:
	double value;

    static const int MAXTHREADS = 10;

public:
    anl::CImplicitXML noiseTree;
    std::map<irr::core::vector3d<int>, TerrainChunk> worldMap;
    std::map<irr::core::vector3d<int>, TerrainChunk>::iterator worldMap_iterator;

    std::map<irr::core::vector3d<int>, TerrainMesh> worldMesh;
    std::map<irr::core::vector3d<int>, TerrainMesh>::iterator worldMesh_iterator;

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
    float GetValue(irr::core::vector3d<int> position);
    int GetMaterial(irr::core::vector3d<int> position);
    void generateMesh(const irr::scene::SViewFrustum * Frustum);
    void FillBackground(irr::core::vector3d<int> tl);
    void MeshBackground(irr::core::vector3d<int> tl);

    void UpdateVoxel(VoxelData & vd);

    void AddBrush(irr::core::vector3df Position);
    void RemoveBrush(irr::core::vector3df Position);

    int GetAltitude(const irr::core::vector3d<int> & Position);
    int GetAltitude(const irr::core::vector3df & Position);
    bool GetFilled(const irr::core::vector3d<int> & Position);
    bool GetFilled(const irr::core::vector3df & Position);

    bool BlockFilled(irr::core::vector3df);
};

class TerrainMesh
{
public:
    int status;

    static const int isolevel = 0.5;

    irr::scene::SMeshBuffer * buffer;
    irr::scene::SMeshBuffer * tempBuffer;
    irr::core::vector3d<int> localPoint;
    ScalarTerrain * parent;

    Unsigned3Array * generatedPoints;

    TerrainMesh(){};

    void Initialize(ScalarTerrain * _parent, irr::core::vector3d<int> tl)
    {
        parent = _parent;
        localPoint = irr::core::vector3d<int>(
            tl.X * dimensions.X,
            tl.Y * dimensions.Y,
            tl.Z * dimensions.Z
        );
        buffer = new irr::scene::SMeshBuffer();
        status = DIRTY;
    };

    void GenerateMesh();
    void NaiveNormals();
    void GenerateSurface(irr::core::vector3d<unsigned> renderBlock, float Values[8], int Materials[8]);
};


#endif