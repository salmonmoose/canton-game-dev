#ifndef VOXELSCENENODE_H
#define VOXELSCENENODE_H

#include <irrlicht.h>
#include "Voxel.h"

static irr::core::vector3d<int> Previous[6] = {
    irr::core::vector3d<int>(0,0,1),
    irr::core::vector3d<int>(0,1,0),
    irr::core::vector3d<int>(0,1,1),
    irr::core::vector3d<int>(1,0,0),
    irr::core::vector3d<int>(1,0,1),
    irr::core::vector3d<int>(1,1,1)
};

class VoxelChunk;
class ChunkMesh;
struct VoxelData;

class VoxelSceneNode : public irr::scene::IMeshSceneNode
{
public:
    irr::core::aabbox3df Box;
    irr::video::SMaterial Material;
    irr::scene::SMesh Mesh;

    std::map<irr::core::vector3d<int>, VoxelChunk> chunkMap;
    std::map<irr::core::vector3d<int>, VoxelChunk>::iterator chunkMapIterator;

    std::map<irr::core::vector3d<int>, ChunkMesh> meshMap;
    std::map<irr::core::vector3d<int>, ChunkMesh>::iterator meshMapIterator;

    anl::CImplicitXML noiseTree;

    irr::io::path vsFileName;
    irr::io::path psFileName;

    irr::s32 terrainMaterial;

    static const int MAXTHREADS = 10;

    int threads;

    VoxelSceneNode(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * manager, irr::s32 id);

    bool Initialize();

/*
* Overridden:
*/
    void render();

    const irr::core::aabbox3df & getBoundingBox() const;

    void setMesh(irr::scene::IMesh *){};

    irr::scene::IMesh * getMesh(){};

    void setReadOnlyMaterials(bool){};

    bool isReadOnlyMaterials() const {};

    void preRenderCalculationsIfNeeded();

    irr::scene::IShadowVolumeSceneNode * addShadowVolumeSceneNode(const irr::scene::IMesh *, irr::s32, bool, irr::f32){};

    void OnRegisterSceneNode();
    void AddBrush(irr::core::vector3df);
    void RemoveBrush(irr::core::vector3df);
    float GetValue(irr::core::vector3d<int>);

    int GetMaterial(irr::core::vector3d<int>);

    const irr::video::SMaterial & getMaterial(irr::u32) const;
    irr::u32 getMaterialCount() const;

    void UpdateVoxel(VoxelData & vd, bool subtract);

    void FillBackground(irr::core::vector3d<int> chunkPos);

    void MeshBackground(irr::core::vector3d<int> chunkPos);

    int GetAltitude(const irr::core::vector3df & Position);

    int GetAltitude(const irr::core::vector3d<int> & Position);
};
#endif