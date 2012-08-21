#include <iostream>
#include <irrlicht.h>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"


static int x_chunk = 24;
static int y_chunk = 24;
static int z_chunk = 24;

using namespace irr;

ScalarTerrain::ScalarTerrain()
{
	tc = TerrainChunk();

	printf("Loading Terrain Document\n");
	terrainConfig.LoadFile("basicTerrain.xml");

	terrainData = terrainConfig.FirstChildElement("document")->FirstChildElement("terrain");

	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;
	printf("Resizing node\n");
	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

	printf("Setting up accidental noise functions\n");
	setupAccidentalNoise();
}

void ScalarTerrain::setupAccidentalNoise() {
    anl::CImplicitSphere thisLayer;

    try {
    	tinyxml2::XMLElement * thisElement = terrainData->FirstChildElement("layer");

        if(thisElement->Attribute("type", "sphere")) {
        	anl::CImplicitSphere thisLayer;
            
            thisLayer.setCenter(
                thisElement->FirstChildElement("center")->DoubleAttribute("x"),
                thisElement->FirstChildElement("center")->DoubleAttribute("y"),
                thisElement->FirstChildElement("center")->DoubleAttribute("z"),
                thisElement->FirstChildElement("center")->DoubleAttribute("u"),
                thisElement->FirstChildElement("center")->DoubleAttribute("v"),
                thisElement->FirstChildElement("center")->DoubleAttribute("w")
            );

            //thisLayer.setRadius(thisElement->FirstChildElement("radius")->DoubleAttribute("radius"));
            
            //lastLayer.setSource(& thisLayer);

        } else if(thisElement->Attribute("type", "fractal")) {
        	anl::CImplicitFractal thisLayer(0,0,0);

        	thisLayer.setNumOctaves(thisElement->FirstChildElement("octaves")->IntAttribute("value"));
        	thisLayer.setFrequency(thisElement->FirstChildElement("frequency")->DoubleAttribute("value"));

        	if(thisElement->Attribute("fractaltype", "FBM")) {
        		thisLayer.setType(anl::FBM);
        	} else if(thisElement->Attribute("fractaltype", "RIDGEDMULTI")) {
				thisLayer.setType(anl::RIDGEDMULTI);
        	} else if(thisElement->Attribute("fractaltype", "BILLOW")) {
				thisLayer.setType(anl::BILLOW);
        	} else if(thisElement->Attribute("fractaltype", "MULTI")) {
				thisLayer.setType(anl::MULTI);
        	} else if(thisElement->Attribute("fractaltype", "HYBRIDMULTI")) {
				thisLayer.setType(anl::HYBRIDMULTI);
        	}
			//setLacunarity(double l);
			//setGain(double g);
			//setOffset(double o);
			//setH(double h);
			//setAllSourceTypes(unsigned int basis_type, unsigned int interp);
			//setSourceType(int which, unsigned int type, unsigned int interp);
			//overrideSource(int which, CImplicitModuleBase *b);
			//resetSource(int which);
			//resetAllSources();	
			//setSeed(unsigned int seed);
        }

    } catch (char * except) {
        printf("Exception raised: %s\n", except);
    }

	for(int z = 0; z < z_chunk; z++) {
		for(int y = 0; y < y_chunk; y++) {
			for(int x = 0; x < x_chunk; x++) {
				value = thisLayer.get(
                    (double) x/x_chunk * 2, 
                    (double) y/y_chunk * 2, 
                    (double) z/z_chunk * 2
                );

				tc.values[x][y][z] = value;

				if(value < -0.5) tc.materials[x][y][z] = 0;
				else if (value < 0) tc.materials[x][y][z] = 1;
				else if (value < 0.5) tc.materials[x][y][z] = 2;
				else tc.materials[x][y][z] = 3;
			}
		}
	}
}