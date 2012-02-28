import random
from moosecore import GameObject
import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math
import random
import noise
#from threading import Thread
#import Queue

import multiprocessing
import logging
import time

BRANCHLOOKUP = {"3":0, "2":1, "-2":2, "-1":3, "1":4, "0":5, "-4":6, "-3":7}

BRANCHMULTIPLIERS = [[ 1, 1, 1], [ 1, 1,-1], [ 1,-1, 1], [ 1,-1,-1], [-1, 1, 1], [-1, 1,-1], [-1,-1, 1], [-1,-1,-1]]

logger = multiprocessing.log_to_stderr()
logger.setLevel(multiprocessing.SUBDEBUG)

'''
 .d88b.   .o88b. d888888b d8b   db  .d88b.  d8888b. d88888b 
.8P  Y8. d8P  Y8 `~~88~~' 888o  88 .8P  Y8. 88  `8D 88'     
88    88 8P         88    88V8o 88 88    88 88   88 88ooooo 
88    88 8b         88    88 V8o88 88    88 88   88 88~~~~~ 
`8b  d8' Y8b  d8    88    88  V888 `8b  d8' 88  .8D 88.     
 `Y88P'   `Y88P'    YP    VP   V8P  `Y88P'  Y8888D' Y88888P 
 '''
class OctNode(GameObject):
    def __init__(self, tree, parent, position, size, data):
        self.tree = tree #FIXME this is implied by parent.tree
        self.parent = parent
        self.position = position #FIXME This is a waste of data position is implied by parent.branches[this.id]
        self.size = size #FIXME this is implied by parent.size/2
        self.bounds = size / 2 #FIXME this is implied by parent.size/4
        self.isLeafNode = True #FIXME this is a waste of data, can be implied by not any branches
        self.data = data
        self.branches = [None, None, None, None, None, None, None, None]

    def __str__(self):
        return (
        "x:" + str(self.position[0]) + 
        ", y:" + str(self.position[1]) + 
        ", z:" + str(self.position[2]) + 
        ", size:" + str(self.size) + 
        ", data:" + str(self.data) +
        ", is leaf node" if self.isLeafNode 
    else ", has branches"
    )

    def draw(self, size):
        #print('node', self.data)
        '''Draw this OctNode, if size is smaller than node, draw sub-nodes'''
        if not self.isLeafNode and size < self.size:
            for leaf in self.branches:
                if leaf: leaf.draw(size)
        elif self.data:
            GameObject.draw_voxel(
                GameObject(),
                self.position[0],
                self.position[1],
                self.position[2],
                self.size,
                [0,1,2,3,4,5] #self.outside()
            )

    def depthEncodeing(self):
        '''If a node's branches are all the same value, set the node to that value and trash branches'''
        if not any(self.branches):
            #There are no branches
            pass

    def outside(self):
        '''is the node outside'''
        iterations = [[ 0, 1, 0],[ 0,-1, 0],[ 0, 0, 1],[ 0, 0,-1],[-1, 0, 0],[ 1, 0, 0]]
        faces = []

        for key, multipliers in enumerate(iterations):
            if not self.dataAtPoint((
                    self.position[0] + (self.size * multipliers[0]),
                    self.position[1] + (self.size * multipliers[1]),
                    self.position[2] + (self.size * multipliers[2])
            )): 
                faces.append(key)
        #TODO: this should return a list of outside faces.
        
        return faces

    def dataAtPoint(self, position):
        '''Return the data at a position'''

        '''if this is the node return it's data'''
        if position == self.position:
            return self.data
        
        if (self.position[0] - self.bounds) < position[0] < (self.position[0] + self.bounds) and \
           (self.position[1] - self.bounds) < position[1] < (self.position[1] + self.bounds) and \
           (self.position[2] - self.bounds) < position[2] < (self.position[2] + self.bounds):
            '''if the node is within this node, find which branch it lies in and get that data'''
            targetBranch = self.findBranch(self.position, position)
            if self.branches[targetBranch]:
                return self.branches[targetBranch].dataAtPoint(position)
            else:
                return False
        elif isinstance(self.parent, OctTree):
            return False
        else:
            return self.parent.dataAtPoint(position)

    def generate(self, resolution, seed):
        if resolution == self.size:
            #if the size is the same, we must be looking this node up, if there is no data, we add some for LOD

            self.data = Terrain.getValue(self.position[0], self.position[1], self.position[2], seed)
        elif self.size > resolution:
            if not any (self.branches):
                over_4 = self.size >> 2
                for key, branch in enumerate(self.branches):
                    self.branches[key] = OctNode(
                        self.tree,
                        self,
                        (
                            self.position[0] + over_4 * BRANCHMULTIPLIERS[key][0],
                            self.position[1] + over_4 * BRANCHMULTIPLIERS[key][1],
                            self.position[2] + over_4 * BRANCHMULTIPLIERS[key][2]
                        ), 
                        self.size >> 1,
                        0
                    )
                self.isLeafNode = False
                if resolution < self.tree.resolution or not self.tree.resolution:
                    self.tree.resolution = resolution

            for key, branch in enumerate(self.branches):
                self.branches[key].generate(resolution, seed)
    
    def fromArray(self, dataMap, size):
        if not any (self.branches):
            over_4 = self.size >> 2
            for key, branch in enumerate(self.branches):
                self.branches[key] = OctNode(
                    self.tree,
                    self,
                    (
                        self.position[0] + over_4 * BRANCHMULTIPLIERS[key][0],
                        self.position[1] + over_4 * BRANCHMULTIPLIERS[key][1],
                        self.position[2] + over_4 * BRANCHMULTIPLIERS[key][2]
                    ), 
                    self.size >> 1,
                    dataMap.pop(0)
                )
            self.isLeafNode = False
            if size < self.tree.resolution or not self.tree.resolution:
                self.tree.resolution = size

        else:
            for key, branch in enumerate(self.branches):
                self.branches[key].fromArray(dataMap, size >> 1)
    


    def toArray(self, size):
        '''Convert a tree to a 1D array'''
        if size == self.size:
            return [self.data]
        else:
            result = []
            for branch in self.branches:

                result.extend(branch.toArray(size))
            return result

    def addNode(self, position, size, data):
        '''if the node we're adding is this one set the data to equal what we have.'''

        if(position, size) == (self.position, self.size):
            self.data = data
        elif size < self.size:
            targetBranch = self.findBranch(self.position, position)
            self.isLeafNode = False
            if not self.branches[targetBranch]:
                self.branches[targetBranch] = OctNode(
                    self.tree,
                    self,
                    (
                        (self.position[0] + (self.size / 4)) if position[0] > self.position[0] else (self.position[0] - (self.size / 4)),
                        (self.position[1] + (self.size / 4)) if position[1] > self.position[1] else (self.position[1] - (self.size / 4)),
                        (self.position[2] + (self.size / 4)) if position[2] > self.position[2] else (self.position[2] - (self.size / 4))
                    ), self.size / 2, 0)
            self.branches[targetBranch].addNode(position, size, data)

    def findBranch(self, parent, child):
            '''Take 2 vectors and return child node id'''
            result = 0
            for i in range(3):
                    if parent[i] <= child[i]:
                            result += (-4 / (i + 1) / 2)
                    else:
                            result += ( 4 / (i + 1) / 2)
            result = BRANCHLOOKUP[str(result)]
            return result
'''
 .d88b.   .o88b. d888888b d888888b d8888b. d88888b d88888b 
.8P  Y8. d8P  Y8 `~~88~~' `~~88~~' 88  `8D 88'     88'     
88    88 8P         88       88    88oobY' 88ooooo 88ooooo 
88    88 8b         88       88    88`8b   88~~~~~ 88~~~~~ 
`8b  d8' Y8b  d8    88       88    88 `88. 88.     88.     
 `Y88P'   `Y88P'    YP       YP    88   YD Y88888P Y88888P 
 '''
class OctTree():
    def __init__(self, map, position, treesize, data):
        self.root = OctNode(self, self, position, treesize, data)
        self.treesize = treesize
        self.resolution = None
        self.position = position
        self.map = map
        self.list = {}

    def __str__(self):
        return (
        "x:" + str(self.position[0]) + 
        ", y:" + str(self.position[1]) + 
        ", z:" + str(self.position[2]) + 
        ", size:" + str(self.treesize) + 
        ", tree:" + str(self.root)
        )

    def toArray(self):
        resolution = self.treesize
        result = []
        while resolution >= self.resolution:
            result.extend(self.root.toArray(resolution))
            resolution = resolution >> 1
        return result

    def fromArray(self, dataMap):
        self.root = OctNode(self, self, self.position, self.treesize, dataMap.pop(0))
        self.resolution = self.treesize
        resolution = self.treesize >> 1
        while len(dataMap) > 0:
            self.root.fromArray(dataMap, resolution)

    def addNode(self, position, size, data):
        self.root.addNode(position, size, data)

    def generate(self, resolution, seed):
        #print('{} generating {} at {}'.format(id(self), resolution, start))
        self.root.generate(resolution, seed)
        #print('{} generated {} at {} started {}'.format(id(self), resolution, self.map.game.ticks, start))

    def updateList(self, size):
        callList = glGenLists(1)
        
        glNewList(callList, GL_COMPILE)
        glBegin(GL_QUADS)
        self.root.draw(size)
        glEnd()
        glEndList()

        self.list[size] = callList
        #print('made calllist at {} units'.format(size))

    def draw(self, size):
        #print(id(self), 'tree', self.resolution)
        resolution = self.resolution #Avoid a race-case in threads
        if not resolution:
            return
        else:
            if resolution <= size:
                if size not in self.list:
                    #print("building mesh at {}".format(size))
                    self.updateList(size)
                glCallList(self.list[size])
            else:
                if self.resolution not in self.list:
                    self.updateList(resolution)
                glCallList(self.list[resolution])

'''
d888888b d88888b d8888b. d8888b.  .d8b.  d888888b d8b   db 
`~~88~~' 88'     88  `8D 88  `8D d8' `8b   `88'   888o  88 
   88    88ooooo 88oobY' 88oobY' 88ooo88    88    88V8o 88 
   88    88~~~~~ 88`8b   88`8b   88~~~88    88    88 V8o88 
   88    88.     88 `88. 88 `88. 88   88   .88.   88  V888 
   YP    Y88888P 88   YD 88   YD YP   YP Y888888P VP   V8P
'''
class Terrain():
    def __init__ (self, map, seed):
        self.seed = seed

    '''return datavalue for a point in 3-space'''
    def getValue3(self,x,y,z):

        x_pos = x / 1024.0
        y_pos = y / 1024.0
        z_pos = z / 1024.0

        heightMap = noise.octave_noise_3d(7, 0.5, 0.25, x_pos, z_pos, self.seed)
        caveMap   = noise.octave_noise_4d(7, 0.5, 0.25, x_pos, z_pos, y_pos, self.seed)

        if heightMap > y_pos and heightMap > caveMap:
            #print('returning landscape')
            return random.randint(1,255)
        else:
            #print('returning void')
            return 0

    def getValue(x, y, z, seed):
        x_pos = x / 1024.0
        y_pos = y / 1024.0
        z_pos = z / 1024.0

        heightMap = noise.octave_noise_3d(7, 0.5, 0.25, x_pos, z_pos, seed)
        caveMap   = noise.octave_noise_4d(7, 0.5, 0.25, x_pos, z_pos, y_pos, seed)

        if heightMap > y_pos and heightMap > caveMap:
            #print('returning landscape')
            return random.randint(1,255)
        else:
            #print('returning void')
            return 0
    getValue = staticmethod(getValue)        

class MapThread(multiprocessing.Process):
    def __init__(self, worldMap):
        print(id(worldMap))
        multiprocessing.Process.__init__(self)
        self.map = worldMap


    def run(self):
        while 1:
            if not self.map.game.running:
                break
            if not self.map.queue.empty():
                args = self.map.queue.get_nowait()
                self.map.worldMap[args[0]].generate(args[1])
            else:
                pass
            #print "generating resolution {} for tree at {}".format(resolution, tree.position)
            #tree.generate(resolution)
            #print "finished tree at {}".format(tree.position)
            #self.map.queue.task_done()


def processMap(args):
    logger.info('Building chunk at {} at a resolution of {} units'.format(args[1], args[2]))
    position = args[0]
    tree = OctTree(None, (position[0], position[1], position[2]), 1024, 0)
    tree.generate(args[2], args[3])

    return (tree.toArray(), args[1])



'''
.88b  d88.  .d8b.  d8888b. 
88'YbdP`88 d8' `8b 88  `8D 
88  88  88 88ooo88 88oodD' 
88  88  88 88~~~88 88~~~   
88  88  88 88   88 88      
YP  YP  YP YP   YP 88    
'''
class Map(GameObject):
    def __init__(self, game = None):
        GameObject.__init__(self, game)

        #self.queue = multiprocessing.Queue()
        
        self.terrain = Terrain(self, 11)

        threading = True

        map_z = 20
        map_x = 20

        self.load_textures()

        self.worldMap = []

        for z in range(map_z):
            for x in range(map_x):
                self.worldMap.append(
                        OctTree(self, (x * 1024, 0, z * 1024), 1024, 0)
                )

        resolution = 1024
        if threading:
            self.pool = multiprocessing.Pool(processes=1)
            while resolution >= 128:
                for key, tree in enumerate(self.worldMap):
                    self.pool.apply_async(processMap, args=((self.worldMap[key].position, key, resolution, self.terrain.seed), ), callback=self.mapCallback)
                resolution = resolution >> 1


        else:
            while resolution >= 16:
                #for key, tree in enumerate(self.worldMap):
                #    tree.generate(resolution)
                self.worldMap[0].generate(resolution)

                resolution = resolution >> 1
        
    def mapCallback(self,args):
        self.worldMap[args[1]].fromArray(args[0])
    

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
            tree.draw(32)

        glDisable(GL_TEXTURE_2D)
        glPopMatrix()
        GameObject.draw(self)


if __name__ == "__main__":
    print "please run pymine instead"