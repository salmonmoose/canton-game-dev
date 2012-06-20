from distutils.core import setup, Extension
import numpy as np

ext = Extension('marchingcubes', sources=['marchingcubesmodule.cpp'])

setup(
	name='marchingcubes', 
	version='1.0', 
	description='Generates triangle set from volume data', 
	include_dirs = [np.get_include()],
	ext_modules=[ext]
)
