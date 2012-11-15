#ifndef TERRAIN_H
#define TERRAIN_H

#include <thread>// Apparently not in windows
//#include "boost/thread.hpp"
#include "pugixml.hpp"

#include "anl.h"
#include <irrlicht.h>
#include "marchingcubes.h"
#include "boost/multi_array.hpp"
#include "brush.h"


typedef boost::multi_array<double, 3> ValueArray;
typedef boost::multi_array<unsigned, 3> MaterialArray;

enum EChunkStatus {
    EMPTY,      //This node has not yet been assigned data
    FILLING,    //Currently assigning data to this node
    FILLED,     //This node needs to be meshed
    DIRTY,      //This node needs to be remeshed
    MESHING,    //This node is currently meshing
    CLEAN       //This node doesn't need
};

struct TerrainLocation
{
public:
    int X,Y,Z;

    TerrainLocation(int x, int y, int z)
    {
        X = x;
        Y = y;
        Z = z;
    }
    
    void set(int x, int y, int z)
    {
        X = x;
        Y = y;
        Z = z;
    }

    bool operator < (const TerrainLocation & tl ) const 
    {
        if(X != tl.X)
            return (X < tl.X);
        if(Y != tl.Y)
            return (Y < tl.Y);

        return (Z < tl.Z);
    }
};

class TerrainChunk
{
public:
    ValueArray * values;
    MaterialArray * materials;
    TerrainLocation * localPoint;
    int status;

    bool empty; //No visible cubes;
    bool obstruct; //At least one horizontal plane is filled


    //irr::scene::SMesh * Mesh;
    irr::scene::SMeshBuffer * buf;

    //FIXME Push this to .cpp file
    TerrainChunk(
        int xDim = 16, int yDim = 16, int zDim = 16,
        int xPos = 0, int yPos = 0, int zPos = 0
        )
    {
        empty = true;
        obstruct = false;

        buf = new irr::scene::SMeshBuffer();
        buf->setBoundingBox(irr::core::aabbox3df(
            (double) (xDim * xPos), (double) (yDim * yPos), (double) (zDim * zPos),
            (double) (xDim * xPos + xDim), (double) (yDim * yPos + yDim), (double) (zDim * zPos + zDim)
        ));
        localPoint = new TerrainLocation(xPos,yPos,zPos);

        status = EMPTY;

    	values = new ValueArray();
    	materials = new MaterialArray();
        values->resize(boost::extents[xDim+1][yDim+1][zDim+1]);
        materials->resize(boost::extents[xDim+1][yDim+1][zDim+1]);
    }

    void FillChunk(anl::CImplicitXML & noiseTree);
    void MeshChunk();

};


class ScalarTerrain
{
private:
	double value;

    static const int MAXTHREADS = 10;

public:
    TerrainChunk tc;
    
    anl::CImplicitXML noiseTree;
    std::map<TerrainLocation, TerrainChunk> worldMap;
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
    void FillBackground(TerrainLocation tl);
    void MeshBackground(TerrainLocation tl);

    bool BlockFilled(irr::core::vector3df);
};

#endif