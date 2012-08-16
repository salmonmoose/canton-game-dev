void WorldManager::generateChunk(const ConstVolumeProxy<MaterialDensityPair44>& proxy, const Region& reg)
{
   MaterialDensityPair44 voxel;
   PolyVox::Vector3DInt32 lowCorner = reg.getLowerCorner();
   PolyVox::Vector3DInt32 upperCorner = reg.getUpperCorner();

   //Form gradient to base land shape off of. This can be altered to change relative height and slant of landscape
   anl::CImplicitGradient landGradient;
   landGradient.setGradient(0.0,0.0,0.0,1.0,0.0,0.0);
   //Generates Fractal noise in order to disturb the ground pattern
   anl::CImplicitFractal groundShape(anl::FBM,anl::GRADIENT,anl::QUINTIC);
   groundShape.setNumOctaves(6);
   groundShape.setFrequency(2);
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

   //Select module performs a step function on the gradient in order to get a clear boundary between earth and sky
   //Change threshold to alter height of landscape based on gradient
   anl::CImplicitSelect landStep;
   landStep.setLowSource(1.0);
   landStep.setHighSource(0.0);
   landStep.setThreshold(0.5);
   landStep.setControlSource(&translate);


   //Set all voxels
   for(double x = reg.getLowerCorner().getX(); x <= reg.getUpperCorner().getX(); x++)
   {
      for(double y = reg.getLowerCorner().getY(); y <= reg.getUpperCorner().getY(); y++)
      {
         for(double z = reg.getLowerCorner().getZ(); z <= reg.getUpperCorner().getZ(); z++)
		   {
            if(landStep.get(x / 32,y / 32,z / 32))
            {
               voxel.setMaterial(0);
		         voxel.setDensity(MaterialDensityPair44::getMaxDensity());
               proxy.setVoxelAt(x, y, z, voxel);
            }
	      }
      }   
   }
}