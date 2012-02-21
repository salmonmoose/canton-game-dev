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

BRANCHLOOKUP = {"3":0, "2":1, "-2":2, "-1":3, "1":4, "0":5, "-4":6, "-3":7}

'''
 .d88b.   .o88b. d888888b d8b   db  .d88b.  d8888b. d88888b 
.8P  Y8. d8P  Y8 `~~88~~' 888o  88 .8P  Y8. 88  `8D 88'     
88    88 8P         88    88V8o 88 88    88 88   88 88ooooo 
88    88 8b         88    88 V8o88 88    88 88   88 88~~~~~ 
`8b  d8' Y8b  d8    88    88  V888 `8b  d8' 88  .8D 88.     
 `Y88P'   `Y88P'    YP    VP   V8P  `Y88P'  Y8888D' Y88888P 
 '''
class OctNode(GameObject):
        def __init__(self, parent, position, size, data):
                self.parent = parent
                self.position = position
                self.size = size
                self.bounds = size / 2
                self.isLeafNode = True
                self.data = data
                self.branches = [None, None, None, None, None, None, None, None]

        def __str__(self):
                return (
                "x:" + str(self.position[0]) + 
                ", y:" + str(self.position[1]) + 
                ", z:" + str(self.position[2]) + 
                ", size:" + str(self.size) + 
                ", data:" + str(self.data) +
                ", is leaf node" if self.isLeafNode else ", is branch")

        def draw(self, size):
                """Draw this OctNode, if size is smaller than node, draw sub-nodes"""
                if not self.isLeafNode and size < self.size:
                        for leaf in self.branches:
                                if leaf: leaf.draw(size)
                elif self.data and self.outside():
                        GameObject.draw_voxel(
                                GameObject(),
                                self.position[0],
                                self.position[1],
                                self.position[2],
                                self.size
                        )

        def depthEncodeing(self):
                '''If a node's branches are all the same value, set the node to that value and trash branches'''
                pass

        def outside(self):
                '''is the node outside'''
                iterations = [[-1, 0, 0],[ 1, 0, 0],[ 0,-1, 0],[ 0, 1, 0],[ 0, 0,-1],[ 0, 0, 1]]
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

        def addNode(self, position, size, data):
                '''if the node we're adding is this one set the data to equal what we have.'''

                if(position, size) == (self.position, self.size):
                        self.data = data
                elif size < self.size:
                        targetBranch = self.findBranch(self.position, position)
                        self.isLeafNode = False
                        if not self.branches[targetBranch]:
                                self.branches[targetBranch] = OctNode(
                                        self,
                                        (
                                                (self.position[0] + (self.size / 4)) if position[0] > self.position[0] else (self.position[0] - (self.size / 4)),
                                                (self.position[1] + (self.size / 4)) if position[1] > self.position[1] else (self.position[1] - (self.size / 4)),
                                                (self.position[2] + (self.size / 4)) if position[2] > self.position[2] else (self.position[2] - (self.size / 4))                                        
                                        ), self.size / 2, 0)
                        self.branches[targetBranch].addNode(position, size, data)

        def findBranch(self, parent, child):
                """Take 2 vectors and return an integer value of child node"""
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
class OctTree:
        def __init__(self, position, worldsize, data):
                self.root = OctNode(self, position, worldsize, data)
                self.worldsize = worldsize

        def addNode(self, position, size, data):
                self.root.addNode(position, size, data)


        def draw(self, size):
                self.root.draw(size)

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

                self.load_textures()

                self.worldMap = OctTree((0,0,0), 1024, 1)

                self.generateWorldMap()

        def draw(self):
                glPushMatrix()
                glEnable(GL_TEXTURE_2D)
                glBegin(GL_QUADS)
                
                draw_nodes = 0
                self.worldMap.draw(64)

                glEnd()
                glDisable(GL_TEXTURE_2D)
                glPopMatrix()
                GameObject.draw(self)

        def generateWorldMap(self):
                mapPosition = (150, 150) #We're working with a single map-grid right now
                resolution = 64
                while resolution >= 64:
                        count = 1024/resolution
                        for z in range(count):
                                for y in range(count):
                                        for x in range(count):
                                                x_pos = ((resolution/2) * (count-1) * -1 + (x * resolution))
                                                y_pos = ((resolution/2) * (count-1) * -1 + (y * resolution))
                                                z_pos = ((resolution/2) * (count-1) * -1 + (z * resolution))

                                                value = noise.octave_noise_3d(
                                                        5, 0.5, 0.04, 
                                                        (mapPosition[0] + x_pos / 1024.0) / 2, 
                                                        (mapPosition[1] + y_pos / 1024.0) / 1,
                                                        (z_pos / 1024.0) / 1
                                                )
                                                if value > 0:
                                                        self.worldMap.addNode(
                                                                (x_pos, y_pos, z_pos), 
                                                                resolution, 
                                                                1
                                                        )
                        resolution /= 2