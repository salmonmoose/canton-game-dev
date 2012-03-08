from array import array
import random
from moosecore import Voxel
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import mooselog
'''
 .d88b.   .o88b. d888888b d888888b d8888b. d88888b d88888b 
.8P  Y8. d8P  Y8 `~~88~~' `~~88~~' 88  `8D 88'     88'     
88    88 8P         88       88    88oobY' 88ooooo 88ooooo 
88    88 8b         88       88    88`8b   88~~~~~ 88~~~~~ 
`8b  d8' Y8b  d8    88       88    88 `88. 88.     88.     
 `Y88P'   `Y88P'    YP       YP    88   YD Y88888P Y88888P 
'''
class OctTree:
    '''new, single object tree'''
    def __init__(self, position, size):
        self.nodeList = array('B')
        self.position = position
        self.size = size
        self.resolution = None
        self.glLists = {}

    def generate(self, blockSize, function):
        if self.resolution > blockSize << 1:
            self.generate(blockSize << 1, function)

        #I don't like this, I'm sure there's a better way.
        count = 0
        start = 0

        while (self.size>>count > blockSize):
            count += 1

        for level in range (count+1):
            length = OctTree.getLength(level)
            start  = OctTree.getStart(level)
            if(len(self.nodeList) <= start):
                self.nodeList.extend([0 for node in range(length)])

            for index in range(start, start+length):
                self.nodeList[index] = function(self.getPosition(index, level))
        if not self.resolution or self.resolution > blockSize:
            self.resolution = blockSize

    def getPosition(self, index, level):
        position = OctTree.deinterleave3(index - OctTree.getStart(level))

        return (
            self.position[0] + (-self.size>>1) + (self.size>>(level+1)) + (position[0] * self.size>>level),
            self.position[1] + (-self.size>>1) + (self.size>>(level+1)) + (position[1] * self.size>>level),
            self.position[2] + (-self.size>>1) + (self.size>>(level+1)) + (position[2] * self.size>>level)
            )

    def getResolution(self):
        length = len(self.nodeList)
        count = 0

        while length > 0:
            length -= OctTree.getLength(count)
            count += 1

        return self.size >> count << 1

    @staticmethod
    def getLength(level):
        return (1 << level) ** 3

    @staticmethod
    def getStart(level):
        start = 0
        for i in range(level):
            start += OctTree.getLength(i)
        return start

    @staticmethod
    def getLevel(size):
        count = 0
        while (self.size >> count > size):
            count += 1

        return count

    def getRange(self, size):
        count = 0
        while (self.size>>count > size):
            count += 1

        return (OctTree.getStart(count), OctTree.getStart(count) + OctTree.getLength(count))

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
    def interleave2(x, y):
        return OctTree.part1by1(x) | (OctTree.part1by1(y) << 1)

    @staticmethod
    def deinterleave2(n):
        return OctTree.unpart1by1(n), OctTree.unpart1by1(n >> 1)

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
        return OctTree.part1by2(x) | (OctTree.part1by2(y) << 1) | (OctTree.part1by2(z) << 2)

    @staticmethod
    def deinterleave3(n):
        return OctTree.unpart1by2(n), OctTree.unpart1by2(n >> 1), OctTree.unpart1by2(n >> 2)

    @staticmethod
    def getNeighbours2(n):
        position = OctTree.deinterleave2(n)
        up    = OctTree.interleave2(position[0],   position[1]-1)
        down  = OctTree.interleave2(position[0],   position[1]+1)
        left  = OctTree.interleave2(position[0]-1, position[1]  )
        right = OctTree.interleave2(position[0]+1, position[1]  )

        return up, down, left, right

    @staticmethod
    def getNeighbours3(n):
        position = OctTree.deinterleave3(n)

        up    = OctTree.interleave3(position[0],   position[1]+1, position[2]  )
        down  = OctTree.interleave3(position[0],   position[1]-1, position[2]  )
        left  = OctTree.interleave3(position[0],   position[1],   position[2]+1)
        right = OctTree.interleave3(position[0],   position[1],   position[2]-1)
        front = OctTree.interleave3(position[0]-1,   position[1],   position[2])
        back  = OctTree.interleave3(position[0]+1,   position[1],   position[2])

        return [up, down, left, right, front, back]

    def bitwiseSearch(position):
        '''returns the octnode at position'''
        if position == self.position:
            return self

        index = 0
        
        if position[0] > self.position[0]:
            index |= 1
        if position[1] > self.position[1]:
            index |= 2
        if position[2] > self.position[2]:
            index |= 4

        return self.branches[index].bitwiseSearch(position)

    def outside(self, size, index):
        dataRange = self.getRange(size)

        faces = []

        for key, neighbour in enumerate(OctTree.getNeighbours3(index - dataRange[0])):
            if neighbour > dataRange[1]-dataRange[0]-1:
                faces.append(key)
            elif self.nodeList[neighbour+dataRange[0]] == 0:
                faces.append(key)

        return faces

    def updateList(self, size):
        callList = glGenLists(1)

        glNewList(callList, GL_COMPILE)
        glBegin(GL_QUADS)

        count = 0
        while (self.size>>count > size):
            count += 1

        dataRange = self.getRange(size)

        for data in range(dataRange[0], dataRange[1]):
            if self.nodeList[data]:
                position = self.getPosition(data, count)
                Voxel.draw(
                    position[0],
                    position[1],
                    position[2],
                    size,
                    self.outside(size, data),
                    self.nodeList[data]
                )
        glEnd()
        glEndList()

        self.glLists[size] = callList

    def draw(self, size):
        #print('node', self.data)
        '''Draw this OctNode, if size is smaller than node, draw sub-nodes'''
        resolution = self.resolution
        if not resolution:
            return
        elif resolution > size:
            self.draw(resolution)
        else:
            count = 0
            while (self.size>>count > size):
                count += 1

            dataRange = self.getRange(size)
            
            if size not in self.glLists:
                self.updateList(size)
            glCallList(self.glLists[size])



    def toArray(self):
        return self.nodeList.tolist()

    def fromArray(self, data):
        if len(data) > 0:
            self.nodeList = array('B')
            self.nodeList.fromlist(data)
            self.resolution = self.getResolution()

def simpleCallback(position):

    return random.randint(0,255)

if __name__ == '__main__':
    tree = OctTree((0,0,0), 1024)
    tree.generate(256, simpleCallback)

    tree.draw(512)

    print (tree.nodeList)