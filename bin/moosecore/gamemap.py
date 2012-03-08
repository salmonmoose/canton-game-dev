import random
from moosecore import GameObject, Voxel
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

BRANCHLOOKUP = {"3":0, "2":1, "-2":2, "-1":3, "1":4, "0":5, "-4":6, "-3":7}

BRANCHMULTIPLIERS = [[ 1, 1, 1], [ 1, 1,-1], [ 1,-1, 1], [ 1,-1,-1], [-1, 1, 1], [-1, 1,-1], [-1,-1, 1], [-1,-1,-1]]

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

from math import sqrt, sin, cos

class Terrain():
    def __init__ (self, map, seed):
        self.seed = seed

    @staticmethod
    def getValue(x, y, z, seed):
        x_pos = x / 1024.0
        y_pos = y / 1024.0
        z_pos = z / 1024.0

        heightMap = noise.octave_noise_3d(7, 0.5, 0.25, x_pos, z_pos, seed)
        caveMap   = noise.octave_noise_4d(7, 0.5, 0.25, x_pos, z_pos, y_pos, seed)
        dirtMap   = noise.octave_noise_3d(7, 0.5, 0.25, x_pos / 2, z_pos / 2, seed ** seed) / 1.02

        if y_pos < sin (heightMap):
            if y_pos < sin(heightMap) - dirtMap:
                return 1
            else:
                return 16
        else:
            return 0


'''
.88b  d88.  .d8b.  d8888b. 
88'YbdP`88 d8' `8b 88  `8D 
88  88  88 88ooo88 88oodD' 
88  88  88 88~~~88 88~~~   
88  88  88 88   88 88      
YP  YP  YP YP   YP 88    
'''
from octtree import OctTree

def processMap(args):
    tree = OctTree((args[0]), 1024)
    tree.fromArray(args[3])
    tree.generate(args[2], terrainCallback)

    result = (args[1], tree.toArray())
    return result

def terrainCallback(position):
    return Terrain.getValue(position[0], position[1], position[2], 11)
    

class Map(GameObject):
    def __init__(self, game = None):
        GameObject.__init__(self, game)

        #self.queue = multiprocessing.Queue()

        self.terrain = Terrain(self, 11)

        threading = True

        map_z = 50
        map_x = 50

        self.load_textures()

        self.worldMap = []

        for z in range(map_z):
            for x in range(map_x):
                self.worldMap.append(
                        OctTree((x * 1024, 0, z * 1024), 1024)
                )

        resolution = 1024
        
        
        if threading: 
            self.pool = multiprocessing.Pool(processes=2)

        while resolution >= 64:
            for key, tree in enumerate(self.worldMap):
                
                if threading:
                    self.pool.apply_async(processMap, ([self.worldMap[key].position, key, resolution, self.worldMap[key].toArray()],), callback=self.mapCallback)
                else:
                    self.mapCallback(processMap((self.worldMap[key].position, key, resolution, self.worldMap[key].toArray())))
            resolution = resolution >> 1

        
    def mapCallback(self,args):
        self.worldMap[args[0]].fromArray(args[1])
        self.worldMap[args[0]].getResolution()

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
            tree.draw(16)

        glDisable(GL_TEXTURE_2D)
        glPopMatrix()
        GameObject.draw(self)


if __name__ == "__main__":
    print "please run pymine instead"