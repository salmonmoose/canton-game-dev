from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *


class MeshGen:
    def __init__(self):
        pass

    def getCallList(self, voxel_list):
        '''Generates a glCallList'''
        pass

class SimpleCubes(MeshGen):
    def __init__(self):
        MeshGen.__init__(self)
        self.wireframe = False

    def getCallList(self, voxel_list):
        pass

class SlicedCubes(MeshGen):
    def __init__(self):
        MeshGen.__init__(self)
        self.wireframe = False

    def getCallList(self, voxel_list):
        
        pass