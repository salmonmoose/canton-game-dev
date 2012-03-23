from array import array
import random
from moosecore import Voxel
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import mooselog
import time
import numpy

def print_timing(func):
    def wrapper(*arg):
        t1 = time.time()
        res = func(*arg)
        t2 = time.time()
        print '%s took %0.3f ms' % (func.func_name, (t2-t1)*1000.0)
        return res
    return wrapper


'''
 .d8b.  d8888b. d8888b.  .d8b.  db    db d8888b. d8888b. 
d8' `8b 88  `8D 88  `8D d8' `8b `8b  d8' VP  `8D 88  `8D 
88ooo88 88oobY' 88oobY' 88ooo88  `8bd8'    oooY' 88   88 
88~~~88 88`8b   88`8b   88~~~88    88      ~~~b. 88   88 
88   88 88 `88. 88 `88. 88   88    88    db   8D 88  .8D 
YP   YP 88   YD 88   YD YP   YP    YP    Y8888P' Y8888D' 
'''

'''Store Nodes in a 3 dimensional z-curve'''

'''
How Neighbours Look:
*------*------*------*
|      |      |      |
|      |      |      |
|      |      |      |
*------*------*------*

'''



class Array3D:

    @print_timing    
    def __init__(self, dimension):
        mooselog.log.info('building array')
        self.nodeList = numpy.zeros(dimension ** 3)
        self.neighbourList = numpy.zeros(dimension ** 3)

    @staticmethod
    def bitwisePosition(index):
        '''returns direction of branch by index'''
        position = (
            1 if index & 1 else -1,
            1 if index & 2 else -1,
            1 if index & 4 else -1
            )
        return position

    @staticmethod
    def part1by1(n):
        n&= 0x0000ffff
        n = (n | (n << 8)) & 0x00FF00FF
        n = (n | (n << 4)) & 0x0F0F0F0F
        n = (n | (n << 2)) & 0x33333333
        n = (n | (n << 1)) & 0x55555555
        return n

    @staticmethod
    def unpart1by1(n):
        n&= 0x55555555
        n = (n ^ (n >> 1)) & 0x33333333
        n = (n ^ (n >> 2)) & 0x0f0f0f0f
        n = (n ^ (n >> 4)) & 0x00ff00ff
        n = (n ^ (n >> 8)) & 0x0000ffff
        return n

    @staticmethod
    def part1by2(n):
        n&= 0x000003ff
        n = (n ^ (n << 16)) & 0xff0000ff
        n = (n ^ (n <<  8)) & 0x0300f00f
        n = (n ^ (n <<  4)) & 0x030c30c3
        n = (n ^ (n <<  2)) & 0x09249249
        return n

    @staticmethod
    def unpart1by2(n):
        n&= 0x09249249
        n = (n ^ (n >>  2)) & 0x030c30c3
        n = (n ^ (n >>  4)) & 0x0300f00f
        n = (n ^ (n >>  8)) & 0xff0000ff
        n = (n ^ (n >> 16)) & 0x000003ff
        return n

    @staticmethod
    def interleave3(x, y, z):
        return Array3D.part1by2(x) | (Array3D.part1by2(y) << 1) | (Array3D.part1by2(z) << 2)

    @staticmethod
    def deinterleave3(n):
        return Array3D.unpart1by2(n), Array3D.unpart1by2(n >> 1), Array3D.unpart1by2(n >> 2)

    @staticmethod
    def getNeighbours3(n):
        position = Array3D.deinterleave3(n)

        up    = Array3D.interleave3(position[0],   position[1]+1, position[2]  )
        down  = Array3D.interleave3(position[0],   position[1]-1, position[2]  )
        left  = Array3D.interleave3(position[0],   position[1],   position[2]+1)
        right = Array3D.interleave3(position[0],   position[1],   position[2]-1)
        front = Array3D.interleave3(position[0]-1,   position[1],   position[2])
        back  = Array3D.interleave3(position[0]+1,   position[1],   position[2])

        return [up, down, left, right, front, back]

    def outside(self, index):
        faces = []

        for key, neighbour in enumerate(Array3D.getNeighbours3(index)):
            if neighbour > len(self.nodeList) -1:
                faces.append(key)
            elif self.nodeList[neighbour] == 0:
                faces.append(key)

        return faces

    def getHeight(x,z,bottom=False):
        '''Returns the topmost point in column of blocks - or bottommost if bottom set to true'''
        pass


    def toArray(self):
        return self.nodeList

    def fromArray(self, data):
        if len(data) > 0:
            self.nodeList = data

def simpleCallback(position):

    return random.randint(0,255)

if __name__ == '__main__':
    tree = Array3D(32)

    print (tree.nodeList)

                                                                                                                                         
