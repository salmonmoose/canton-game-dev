#pragma once
#include <boost/multi_array.hpp>

typedef boost::multi_array<double, 3> ValueArray;
typedef boost::multi_array<int, 3> MaterialArray;

struct TerrainChunk {
	ValueArray values;
	MaterialArray materials;
	int worldLocX, worldLocY, worldLocZ;

	TerrainChunk()
	{
		values.resize(boost::extents[1][1][1]);
		materials.resize(boost::extents[1][1][1]);
	}
};