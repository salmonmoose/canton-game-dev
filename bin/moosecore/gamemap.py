import random
from moosecore import GameObject, Voxel, SmoothVox
import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math
import random
#from threading import Thread
#import Queue
import noise
import multiprocessing

import time

import mooselog

log = multiprocessing.log_to_stderr()
log.setLevel(multiprocessing.SUBDEBUG)

'''
d888888b d88888b d8888b. d8888b.  .d8b.  d888888b d8b   db 
`~~88~~' 88'     88  `8D 88  `8D d8' `8b   `88'   888o  88 
   88    88ooooo 88oobY' 88oobY' 88ooo88    88    88V8o 88 
   88    88~~~~~ 88`8b   88`8b   88~~~88    88    88 V8o88 
   88    88.     88 `88. 88 `88. 88   88   .88.   88  V888 
   YP    Y88888P 88   YD 88   YD YP   YP Y888888P VP   V8P
'''

from math import sin

class Terrain():
    def __init__ (self, map, seed):
        self.seed = seed

    @staticmethod
    def getValue(x, y, z, seed):

        heightMap = noise.octave_noise_3d(7, 0.5, 0.25, x, y, seed)

        caveMap   = noise.octave_noise_4d(7, 0.5, 0.25, x, z, y, seed)
        dirtMap   = noise.octave_noise_3d(7, 0.5, 0.25, x, z, seed) 

        if sin (y) < heightMap:
            if sin(y) < heightMap - dirtMap:
                return 16
            else:
                return 1
        else:
            return 0

    @staticmethod
    def getHeight(x,z,seed):
        heightMap = noise.octave_noise_3d(7, 0.5, 0.25, x, z, seed)
        return (
            noise.octave_noise_3d(7, 0.5, 0.25, x, z, seed),
            noise.octave_noise_3d(7, 0.5, 0.25, x, z, seed)
            )


'''
.88b  d88.  .d8b.  d8888b. 
88'YbdP`88 d8' `8b 88  `8D 
88  88  88 88ooo88 88oodD' 
88  88  88 88~~~88 88~~~   
88  88  88 88   88 88      
YP  YP  YP YP   YP 88    
'''
from array3d import Array3D

def processMap(args):
    tree = MapTree((args[0]), 1024)
    tree.fromArray(args[3])
    tree.generate(args[2], terrainCallback)
    result = (args[1], tree.toArray())
    return result

def terrainCallback(position):
    return Terrain.getValue(position[0], position[1], position[2], 11)
    

class Map(GameObject):
    def __init__(self, game = None):
        GameObject.__init__(self, game)

        self.terrain = Terrain(self, 11)

        threading = False

        map_z = 1
        map_x = 1

        self.load_textures()

        self.worldMap = []

        for z in range(map_z):
            for x in range(map_x):
                self.worldMap.append(
                        MapTree((x * 1024, 0, z * 1024), 1024)
                )

        resolution = 128
        
        
        if threading: 
            self.pool = multiprocessing.Pool(processes=2)

        for key, tree in enumerate(self.worldMap):
            if threading:
                self.pool.apply_async(processMap, ([self.worldMap[key].position, key, resolution, self.worldMap[key].toArray()],), callback=self.mapCallback)
            else:
                self.mapCallback(processMap((self.worldMap[key].position, key, resolution, self.worldMap[key].toArray())))
        
    def mapCallback(self,args):
        self.worldMap[args[0]].fromArray(args[1])

    def update(self):
        pass
        '''
        for z in range(map_z):
            for x in range(map_x):
                self.worldMap[map_z + map_x * z].generate(1024)
        '''
        #check how the queue is going

        #check how the rendering near the player is

        #add some stuff to the queue


    def draw(self):
        glPushMatrix()
        glEnable(GL_TEXTURE_2D)
        

        for tree in self.worldMap:
            tree.draw(128)

        glDisable(GL_TEXTURE_2D)
        glPopMatrix()
        GameObject.draw(self)

'''
.88b  d88.  .d8b.  d8888b. d888888b d8888b. d88888b d88888b 
88'YbdP`88 d8' `8b 88  `8D `~~88~~' 88  `8D 88'     88'     
88  88  88 88ooo88 88oodD'    88    88oobY' 88ooooo 88ooooo 
88  88  88 88~~~88 88~~~      88    88`8b   88~~~~~ 88~~~~~ 
88  88  88 88   88 88         88    88 `88. 88.     88.     
YP  YP  YP YP   YP 88         YP    88   YD Y88888P Y88888P 
'''                                                             
'''Abstract Array3D so that I can reuse the code for other objects'''
class MapTree:
    def __init__(self, position, size):
        self.tree = Array3D(32)
        self.position = position
        self.size = size
        self.resolution = None
        self.glLists = {}

    def draw(self, size):
        '''Draw this OctNode, if size is smaller than node, draw sub-nodes'''
        if not len(self.tree.nodeList):
            return

        if size not in self.glLists:
            self.updateList(size)
        glCallList(self.glLists[size])

    def getRange(self, size):
        count = 0
        while (self.size>>count > size):
            count += 1

        return (Array3D.getStart(count), Array3D.getStart(count) + Array3D.getLength(count))

    def updateList(self, dimension):
        callList = glGenLists(1)

        glNewList(callList, GL_COMPILE)
        glBegin(GL_QUADS)

        for index, value in enumerate(self.tree.nodeList):
            if self.tree.nodeList[index]:
                position = self.getPosition(index, dimension)
                SmoothVox.draw(
                    position[0],
                    position[1],
                    position[2],
                    dimension,
                    self.tree.outside(index),
                    self.tree.nodeList[index]
                )
        glEnd()
        glEndList()

        self.glLists[dimension] = callList

    def toArray(self):
        return self.tree.toArray()

    def fromArray(self, arg):
        self.tree.fromArray(arg)

    def generate(self, dimension, function):
        for z in range(dimension):
            for x in range(dimension):
                height_map = Terrain.getHeight(x,z,11)
                



        for index in range(((self.size / dimension) ** 3)):
            self.tree.nodeList[index] = function(self.getPosition(index, dimension))

    def getPosition(self, index, dimension):
        position = Array3D.deinterleave3(index)

        return (
            self.position[0] + (-self.size>>1) + (dimension>>1) + (position[0] * dimension),
            self.position[1] + (-self.size>>1) + (dimension>>1) + (position[1] * dimension),
            self.position[2] + (-self.size>>1) + (dimension>>1) + (position[2] * dimension)
            )

if __name__ == "__main__":
    print "please run pymine instead"