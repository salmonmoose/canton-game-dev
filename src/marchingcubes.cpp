#include "marchingcubes.h"

using namespace irr;

MCubeMesh::MCubeMesh(TerrainChunk & in_tc) : Mesh(0)
{
//FIXME: This is all fucked up
//There is no need to clone the object, a reference would work just as well
//But C++ is hard.
tc = TerrainChunk();
printf("this is a test, %d\n", 0);

Mesh = new scene::SMesh();

printf("this is a test, %d\n", 1);

tc.values.resize(boost::extents
    [in_tc.values.shape()[0]]
    [in_tc.values.shape()[1]]
    [in_tc.values.shape()[2]]
);  

tc.materials.resize(boost::extents
    [in_tc.materials.shape()[0]]
    [in_tc.materials.shape()[1]]
    [in_tc.materials.shape()[2]]
);

printf("this is a test, %d\n", 2);

tc.values = in_tc.values;
tc.materials = in_tc.materials;

printf("You fed me a chunk this big: %d %d %d\n",
        (int)in_tc.values.shape()[0],
        (int)in_tc.values.shape()[1],
        (int)in_tc.values.shape()[2]);
}

MCubeMesh::~MCubeMesh()
{
Mesh->drop();
}

void MCubeMesh::init(video::IVideoDriver *driver)
{
int xDim = tc.values.shape()[0];
int yDim = tc.values.shape()[1];
int zDim = tc.values.shape()[2];

int x,y,z,i,cubeIndex,cacheIndex,ntriang;
float pointVals[8];
int colorVals[8];
video::S3DVertex vertList[12];
float isolevel = 0.5;
float mu;
    core::vector3df tmpVec3D;

int uSpace = yDim * zDim * (xDim -1);
int vSpace = zDim * xDim * (yDim -1);
int wSpace = xDim * yDim * (zDim -1);

scene::SMeshBuffer *buf = 0;

buf = new scene::SMeshBuffer();
Mesh->addMeshBuffer(buf);

buf->drop();

buf->Vertices.clear();
buf->Indices.clear();

for (z = 0; z < zDim -1; z++) {
    for (y = 0; y < yDim -1; y++) {
        for (x = 0; x < xDim -1; x++) {
            //Grab density and color data
            cubeIndex = 0;
            for (i = 0; i < 8; i++) {
                pointVals[i] = tc.values[x + (int)points[i].X][y + (int)points[i].Y][z + (int)points[i].Z];
                colorVals[i] = tc.materials[x + (int)points[i].X][y + (int)points[i].Y][z + (int)points[i].Z];
                if(pointVals[i] > isolevel) cubeIndex |= (1 << i);
            }
            tmpVec3D = core::vector3df((double) x, (double) y, (double) z);

                //Generate verts needed by this voxel.
            for (i = 0; i < 12; i++) {
                if (edgeTable[cubeIndex] & (1 << i))
                {
                    printf("pointvals %f %f\n", pointVals[edges[i][0]], pointVals[edges[i][1]]);


                    mu = (isolevel - pointVals[edges[i][0]]) / (pointVals[edges[i][1]] - pointVals[edges[i][0]]);
                    
                    vertList[i].Pos.set(
                        tmpVec3D.X + (points[edges[i][0]].X + mu * (points[edges[i][1]].X - points[edges[i][0]].X)),
                        tmpVec3D.Y + (points[edges[i][0]].Y + mu * (points[edges[i][1]].Y - points[edges[i][0]].Y)),
                        tmpVec3D.Z + (points[edges[i][0]].Z + mu * (points[edges[i][1]].Z - points[edges[i][0]].Z))
                        );

                    vertList[i].Color.set(
                        colors[colorVals[edges[i][0]]].r + mu * (colors[colorVals[edges[i][1]]].r - colors[colorVals[edges[i][0]]].r),
                        colors[colorVals[edges[i][0]]].g + mu * (colors[colorVals[edges[i][1]]].g - colors[colorVals[edges[i][0]]].g),
                        colors[colorVals[edges[i][0]]].b + mu * (colors[colorVals[edges[i][1]]].b - colors[colorVals[edges[i][0]]].b),
                        colors[colorVals[edges[i][0]]].a + mu * (colors[colorVals[edges[i][1]]].a - colors[colorVals[edges[i][0]]].a)
                        );
                    
                    vertList[i].TCoords.set(0.0,0.0);

                    printf("added point at (%f, %f, %f)\n",
                        vertList[i].Pos.X,
                        vertList[i].Pos.Y,
                        vertList[i].Pos.Z);
                }
            }

                for (i=0;triTable[cubeIndex][i]!=-1;i++) {
                    buf->Indices.push_back(buf->Vertices.size());
                    buf->Vertices.push_back(vertList[triTable[cubeIndex][i]]);
                }
        }
    }
}

for (i = 0; i < buf->Vertices.size(); i += 3) {
    tmpVec3D = (buf->Vertices[i+1].Pos - buf->Vertices[i].Pos).crossProduct(buf->Vertices[i].Pos - buf->Vertices[i + 2].Pos);
    buf->Vertices[i].Normal = tmpVec3D;
    buf->Vertices[i+1].Normal = tmpVec3D;
    buf->Vertices[i+2].Normal = tmpVec3D;
}

buf->recalculateBoundingBox();
}
