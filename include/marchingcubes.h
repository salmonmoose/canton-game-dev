#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "terrain.h"

typedef boost::multi_array<double, 3> Double3Array;
typedef boost::multi_array<float, 3> Float3Array;
typedef boost::multi_array<unsigned, 3> Unsigned3Array;

void generateIsoSurface(
    irr::scene::SMeshBuffer & Mesh, 
    Float3Array & values, 
    Unsigned3Array & materials,
    int x_offset, int y_offset, int z_offset
    );

#endif