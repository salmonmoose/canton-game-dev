#include "marchingcubes.h"
#include <vector>

using namespace irr;

void generateIsoSurface(
        scene::SMeshBuffer& buf, 
        Double3Array & values, 
        Unsigned3Array & materials,
        int x_offset, int y_offset, int z_offset
    )
{
    buf.Vertices.clear();
    buf.Indices.clear();

    const int xDim = values.shape()[0];
    const int yDim = values.shape()[1];
    const int zDim = values.shape()[2];

    int x,y,z,cubeIndex,cacheIndex,ntriang;
    float pointVals[8];
    int colorVals[8];
    irr::core::vector3d<unsigned> vertList[12];
    video::S3DVertex tmpS3DVertex;
    int vertIndexList[12];

    float isolevel = 0.5;
    float mu;

    int index;
    int indexSize = (xDim+1) * (yDim+1) * (zDim+1) * 3;

    core::vector3df tmpVec3D;

    /*
    TODO: There's a better way of doing this;

    http://http.developer.nvidia.com/GPUGems3/gpugems3_ch01.html

    We can get normal data from the noise.

        float d = 1.0/(float)voxels_per_block;  
        float3 grad;  
        grad.x = density_vol.Sample(TrilinearClamp, uvw + float3( d, 0, 0)) -  
                 density_vol.Sample(TrilinearClamp, uvw + float3(-d, 0, 0));  
        grad.y = density_vol.Sample(TrilinearClamp, uvw + float3( 0, d, 0)) -  
                 density_vol.Sample(TrilinearClamp, uvw + float3( 0,-d, 0));  
        grad.z = density_vol.Sample(TrilinearClamp, uvw + float3( 0, 0, d)) -  
                 density_vol.Sample(TrilinearClamp, uvw + float3( 0, 0,-d));  
        output.wsNormal = -normalize(grad);

    We can build our mesh faster:

    list_nonempty_cells - Each voxel is either above or below a threashold, list only voxels that contain enough data.

    list_verts_to_generate - March nonempty_cells, and look only at minor edges (those starting at (0,0,0)) (edges 0,3,8)

    these points can be cached into a multi-array, no need to lookup existing points, as we know they'll exist.

    MAJOR REWRITE.
    */

    Unsigned3Array * generatedPoints;

    generatedPoints = new Unsigned3Array();
    generatedPoints->resize(boost::extents[xDim][yDim][zDim]);

    //std::vector<int> generatedPoints(indexSize, -1);

    for (z = 0; z < zDim -1; z++) {
        for (y = 0; y < yDim -1; y++) {
            for (x = 0; x < xDim -1; x++) {
                
                //Grab density and color data
                cubeIndex = 0;
                for (int i = 0; i < 8; i++) {
                    pointVals[i] = values[x + points[i].X][y + points[i].Y][z + points[i].Z];
                    colorVals[i] = materials[x + points[i].X][y + points[i].Y][z + points[i].Z];
                    if(pointVals[i] > isolevel) cubeIndex |= (1 << i);
                }

                //Generate verts needed by this voxel.
                for (int i = 0; i < 12; i++) {

                    index = ((points[edges[i][0]].X + x)) +
                            ((points[edges[i][0]].Y + y) * xDim) +
                            ((points[edges[i][0]].Z + z) * xDim * yDim);
                    
                    int _x = points[edges[i][0]].X + x;
                    int _y = points[edges[i][0]].Y + y;
                    int _z = points[edges[i][0]].Z + z;


                    if(edgeTable[cubeIndex] & (1 << i)) 
                    {
                        if ((*generatedPoints)[_x][_y][_z] == NULL)
                        {
                            mu = (isolevel - pointVals[edges[i][0]]) / (pointVals[edges[i][1]] - pointVals[edges[i][0]]);
                            
                            tmpS3DVertex.Pos.set(
                                x_offset + x + (points[edges[i][0]].X + mu * (points[edges[i][1]].X - points[edges[i][0]].X)),
                                y_offset + y + (points[edges[i][0]].Y + mu * (points[edges[i][1]].Y - points[edges[i][0]].Y)),
                                z_offset + z + (points[edges[i][0]].Z + mu * (points[edges[i][1]].Z - points[edges[i][0]].Z))
                            );

                            tmpS3DVertex.Color.set(
                                colors[colorVals[edges[i][0]]].r + mu * (colors[colorVals[edges[i][1]]].r - colors[colorVals[edges[i][0]]].r),
                                colors[colorVals[edges[i][0]]].g + mu * (colors[colorVals[edges[i][1]]].g - colors[colorVals[edges[i][0]]].g),
                                colors[colorVals[edges[i][0]]].b + mu * (colors[colorVals[edges[i][1]]].b - colors[colorVals[edges[i][0]]].b),
                                colors[colorVals[edges[i][0]]].a + mu * (colors[colorVals[edges[i][1]]].a - colors[colorVals[edges[i][0]]].a)
                            );

                            tmpS3DVertex.TCoords.set(0.0,0.0);

                            tmpS3DVertex.Normal.set(0.0,0.0,0.0);

                            //We don't need to re-generate this point
                            (*generatedPoints)[_x][_y][_z] = buf.Vertices.size();
                            
                            buf.Vertices.push_back(tmpS3DVertex); //FIXME this should just build the vertex here and now.
                        }
                        else
                        {

                        }
                        
                        vertList[i] = irr::core::vector3d<unsigned>(_x,_y,_z);
                    }
                }

                for (int i=0; triTable[cubeIndex][i] != -1; i++) {
                    int _x = vertList[triTable[cubeIndex][i]].X;
                    int _y = vertList[triTable[cubeIndex][i]].Y;
                    int _z = vertList[triTable[cubeIndex][i]].Z;

                    buf.Indices.push_back((*generatedPoints)[_x][_y][_z]);
                }

            }
        }
    }

    for (unsigned i = 0; i < buf.Indices.size(); i += 3) {
        tmpVec3D = (
            buf.Vertices[buf.Indices[i+1]].Pos - buf.Vertices[buf.Indices[i]].Pos
        ).crossProduct(
            buf.Vertices[buf.Indices[i]].Pos - buf.Vertices[buf.Indices[i + 2]].Pos
        );

        buf.Vertices[buf.Indices[i]].Normal   = buf.Vertices[buf.Indices[i]].Normal + tmpVec3D;
        buf.Vertices[buf.Indices[i+1]].Normal = buf.Vertices[buf.Indices[i+1]].Normal + tmpVec3D;
        buf.Vertices[buf.Indices[i+2]].Normal = buf.Vertices[buf.Indices[i+2]].Normal + tmpVec3D;
    }

    for (unsigned i = 0; i < buf.Vertices.size(); i++)
    {
        buf.Vertices[i].Normal.normalize();
    }

    buf.setDirty();
}