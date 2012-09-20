#ifndef TERRAIN_H
#define TERRAIN_H
#include <iostream>
#include "tinyxml2.h"
#include "pugixml.hpp"


#include "anl.h"
#include <irrlicht.h>
#include "marchingcubes.h"
#include <boost/multi_array.hpp>

typedef boost::multi_array<double, 3> ValueArray;
typedef boost::multi_array<int, 3> MaterialArray;

enum EChunkStatus
{
    C_NEW,  //New Chunk no data, no mesh
    C_DIRTY, //Chunk, data does not match mesh
    C_CLEAN //Chunk data matches mesh
};

struct TerrainChunk {
    ValueArray * values;
    MaterialArray * materials;
    
    int status;

    irr::scene::SMesh * Mesh;

    TerrainChunk() : status(C_NEW)
    {
    	values = new ValueArray();
    	materials = new MaterialArray();
    	printf("Spawning New Chunk\n");
        status = C_NEW;
        values->resize(boost::extents[16][16][16]);
        materials->resize(boost::extents[16][16][16]);
    }
};

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

class ScalarTerrain
{
private:
	double value;

public:
    TerrainChunk tc;
    
    anl::CImplicitXML noiseTree;
    std::map<TerrainLocation, TerrainChunk> worldMap;

    irr::scene::SMesh Mesh;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void renderChunk(TerrainChunk &tc);
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
	void generateNavMesh();
	void getMesh(/*frustum*/);
};

#endif