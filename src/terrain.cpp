#include <iostream>
#include <list>

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

	pugi::xml_parse_result result = terrainConfig.load_file("basicTerrain.xml");

	terrainData = terrainConfig.child("document").child("terrain");

	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;
	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

	setupAccidentalNoise();
}

void ScalarTerrain::setupAccidentalNoise() {
	std::vector<anl::CImplicitModuleBase *> layers;
	anl::CImplicitModuleBase * tmp;

    std::map<std::string, anl::CImplicitModuleBase *> noiseTree;
    std::map<std::string, anl::CImplicitModuleBase *> :: iterator noiseIterator;

    try {
        for (
            pugi::xml_node layer = terrainData.child("layer"); layer; layer = layer.next_sibling("layer")) {

        	if(strcmp(layer.attribute("type").value(), "sphere") == 0) {
                tmp = new anl::CImplicitSphere();

                dynamic_cast<anl::CImplicitSphere*>(tmp)->setCenter(
                	layer.child("center").attribute("x").as_double(),
                	layer.child("center").attribute("y").as_double(),
                	layer.child("center").attribute("z").as_double(),
                	layer.child("center").attribute("u").as_double(),
                	layer.child("center").attribute("v").as_double(),
                	layer.child("center").attribute("w").as_double()
                );

                dynamic_cast<anl::CImplicitSphere*>(tmp)->setRadius(layer.child("radius").attribute("value").as_double());

            } else if(strcmp(layer.attribute("type").value(), "fractal") == 0) {
            	tmp = new anl::CImplicitFractal(
                    anl::CImplicitFractal::FractalMap.find(layer.attribute("fractaltype").value())->second,
                    anl::CImplicitBasisFunction::BasisMap.find(layer.attribute("basistype").value())->second,
                    anl::CImplicitBasisFunction::InterpMap.find(layer.attribute("interptype").value())->second
                );
            } else if(strcmp(layer.attribute("type").value(), "blend") == 0) {
                tmp = new anl::CImplicitBlend();
            } else {
            	printf("Layer type not found\n");
            }

            printf("Adding node to tree with name: %s\n", layer.child_value("name"));

            noiseTree.insert(std::pair<std::string, anl::CImplicitModuleBase *>(layer.child_value("name"), tmp));
        }
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }

    noiseIterator = noiseTree.end();

    noiseIterator--;

    printf("value is %s\n", noiseIterator->first.c_str());

    noiseIterator--;

    printf("value is %s\n", noiseIterator->first.c_str());

    noiseIterator--;

    printf("value is %s\n", noiseIterator->first.c_str());

    printf("Render element %s\n", terrainData.child_value("render"));

    try {
		for(int z = 0; z < z_chunk; z++) {
			for(int y = 0; y < y_chunk; y++) {
				for(int x = 0; x < x_chunk; x++) {
                    value = noiseTree.find(terrainData.child_value("render"))->second->get(
	                    (double) x/x_chunk * 2, 
	                    (double) y/y_chunk * 2, 
	                    (double) z/z_chunk * 2
	                );

					tc.values[x][y][z] = value;

                    //printf("value at position (%d,%d,%d) is %f\n",x,y,z,value);

					if(value < -0.5) tc.materials[x][y][z] = 0;
					else if (value < 0) tc.materials[x][y][z] = 1;
					else if (value < 0.5) tc.materials[x][y][z] = 2;
					else tc.materials[x][y][z] = 3;
				}
			}
		}
	} catch (char * exception) {
		printf("Exception raised: %s\n", exception);
	}
}