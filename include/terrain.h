#ifndef TERRAIN_H
#define TERRAIN_H
#include <iostream>
#include <thread>
#include "tinyxml2.h"
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

    irr::scene::SMesh * Mesh;

    bool clean;
    //FIXME Push this to .cpp file
    TerrainChunk(
        int xDim = 32, int yDim = 32, int zDim = 32,
        int xPos = 0, int yPos = 0, int zPos = 0
        )
    {
        localPoint = new TerrainLocation(xPos,yPos,zPos);
        clean = false;
    	values = new ValueArray();
    	materials = new MaterialArray();
    	printf("Spawning New Chunk\n");
        values->resize(boost::extents[xDim+1][yDim+1][zDim+1]);
        materials->resize(boost::extents[xDim+1][yDim+1][zDim+1]);
    }

    void renderChunk(anl::CImplicitXML & noiseTree);

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
	//void renderChunk(TerrainChunk &tc);
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
	void generateNavMesh();
	void getMesh(/*frustum*/);
    void generateMesh();
    void GenerateBackground(TerrainLocation tl);
};

#endif