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
	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;

	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

	//printf("Resizing chunk, %d", 0);

	setupAccidentalNoise();

	//generateChunk(0,0,0);

	//trashNoise();
}

void ScalarTerrain::setupAccidentalNoise() {
	//Form gradient to base land shape off of. This can be altered to change relative height and slant of landscape
	anl::CImplicitGradient landGradient;
	landGradient.setGradient(0.0,0.0,0.0,1.0,0.0,0.0);
	//Generates Fractal noise in order to disturb the ground pattern
	anl::CImplicitFractal groundShape(anl::FBM,anl::GRADIENT,anl::QUINTIC);
	groundShape.setNumOctaves(2);
	groundShape.setFrequency(1);
	//Scale Offset makes terrain flatter or more mountainous
	anl::CImplicitScaleOffset groundScale(0.5, 0.0);
	groundScale.setSource(&groundShape);
	//Eliminate Floating Islands
	anl::CImplicitScaleDomain yGroundScale;
	yGroundScale.setYScale(0.0);
	yGroundScale.setSource(&groundScale);
	//Links generation together
	anl::CImplicitTranslateDomain translate;
	translate.setSource(&landGradient);
	translate.setYAxisSource(&yGroundScale);
	//translate.setYAxisSource(&groundShape);


	anl::CImplicitSphere sphereGradient;
	sphereGradient.setCenter(0.5,0.5,0.5, 0.5,0.5,0.5);

	//Select module performs a step function on the gradient in order to get a clear boundary between earth and sky
	//Change threshold to alter height of landscape based on gradient
	anl::CImplicitSelect landStep;
	landStep.setLowSource(1.0);
	landStep.setHighSource(0.0);
	landStep.setThreshold(0.5);
	landStep.setControlSource(&translate);

	for(int z = 0; z < z_chunk; z++) {
		for(int y = 0; y < y_chunk; y++) {
			for(int x = 0; x < x_chunk; x++) {
				value = translate.get(
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