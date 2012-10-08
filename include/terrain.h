#ifndef TERRAIN_H
#define TERRAIN_H
#include <thread>// Apparently not in windows
//#include "boost/thread.hpp"
#include "pugixml.hpp"

#include "anl.h"
#include <irrlicht.h>
#include "marchingcubes.h"
#include "boost/multi_array.hpp"


typedef boost::multi_array<double, 3> ValueArray;
typedef boost::multi_array<int, 3> MaterialArray;

struct TerrainLocation
{
    int X,Y,Z;

    TerrainLocation(int x, int y, int z) : X(x), Y(y), Z(z) {}

    bool operator < (const TerrainLocation & tl ) const {
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

    //irr::scene::SMesh * Mesh;
    irr::scene::SMeshBuffer * buf;
    bool clean;
    bool filled;
    bool filling;
    bool meshing;
    //FIXME Push this to .cpp file
    TerrainChunk(
        int xDim = 32, int yDim = 32, int zDim = 32,
        int xPos = 0, int yPos = 0, int zPos = 0
        )
    {
        buf = new irr::scene::SMeshBuffer();
        buf->setBoundingBox(irr::core::aabbox3df(
            (double) (xDim * xPos), (double) (yDim * yPos), (double) (zDim * zPos),
            (double) (xDim * xPos + xDim), (double) (yDim * yPos + yDim), (double) (zDim * zPos + zDim)
        ));
        localPoint = new TerrainLocation(xPos,yPos,zPos);
        clean = false;
        filled = false;
        filling = false;
        meshing = false;

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

public:
    TerrainChunk tc;
    
    anl::CImplicitXML noiseTree;
    std::map<TerrainLocation, TerrainChunk> worldMap;
    irr::io::path vsFileName;
    irr::io::path psFileName;
    irr::scene::SMesh Mesh;
    irr::video::SMaterial Material;
    irr::s32 terrainMaterial;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
	void generateNavMesh();
	void getMesh(/*frustum*/);
    void generateMesh(irr::core::vector3df center);
    void GenerateBackground(TerrainLocation tl);
    void MeshBackground(TerrainLocation tl);
};

#endif