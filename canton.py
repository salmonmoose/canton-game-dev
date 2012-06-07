from math import pi, sin, cos

from direct.showbase.ShowBase import ShowBase
from direct.gui.DirectGui import *
from direct.directtools.DirectGeometry import LineNodePath
from direct.task import Task
from panda3d.core import *

import numpy

import marchingCubes

import draw

points = [
    [0,0,0],
    [1,0,0],
    [1,0,1],
    [0,0,1],
    [0,1,0],
    [1,1,0],
    [1,1,1],
    [0,1,1]
]

vertNormal = [
    [ 1.0, 1.0, 1.0],
    [-1.0, 1.0, 1.0],
    [-1.0, 1.0,-1.0],
    [ 1.0, 1.0,-1.0],
    [ 1.0,-1.0, 1.0],
    [-1.0,-1.0, 1.0],
    [-1.0,-1.0,-1.0],
    [ 1.0,-1.0,-1.0],
]

edgeNormal = [
    [ 0.0, 1.0, 1.0],
    [-1.0, 1.0, 0.0],
    [ 0.0, 1.0,-1.0],
    [ 1.0, 1.0, 0.0],
    [ 0.0,-1.0, 1.0],
    [-1.0,-1.0, 0.0],
    [ 0.0,-1.0,-1.0],
    [ 1.0,-1.0, 0.0],
    [ 1.0, 0.0, 1.0],
    [-1.0, 0.0, 1.0],
    [-1.0, 0.0,-1.0],
    [ 1.0, 0.0,-1.0]
]

edgeUVW = [
    [0.5,0.0,0.0],
    [1.0,0.0,0.5],
    [0.5,0.0,1.0],
    [0.0,0.0,0.5],
    [0.5,1.0,0.0],
    [1.0,1.0,0.5],
    [0.5,1.0,1.0],
    [0.0,1.0,0.5],
    [0.0,0.5,0.0],
    [1.0,0.5,0.0],
    [1.0,0.5,1.0],
    [0.0,0.5,1.0]
]

'''
      .oooooo.                             .                         
     d8P'  `Y8b                          .o8                         
    888           .oooo.   ooo. .oo.   .o888oo  .ooooo.  ooo. .oo.   
    888          `P  )88b  `888P"Y88b    888   d88' `88b `888P"Y88b  
    888           .oP"888   888   888    888   888   888  888   888  
    `88b    ooo  d8(  888   888   888    888 . 888   888  888   888  
     `Y8bood8P'  `Y888""8o o888o o888o   "888" `Y8bod8P' o888o o888o 
'''                                                                  
class Canton(ShowBase):
    
    def __init__(self):
        loadPrcFile('canton.prc')
        ShowBase.__init__(self)

        base.disableMouse()        

        self.title = OnscreenText(
            text='Canton indev',
            style=1, fg=(1,1,1,1), pos=(0.5, -0.95), scale = .07
        )

        self.volume = OnscreenText(
            text='0',
            style=1, fg=(1,1,1,1), pos=(-0.95, -0.95), scale = .07, mayChange=True
        )

        self.mouseStat = OnscreenText(
            text='',
            style=1, fg=(1,1,1,1), pos=(-0.95, -0.9), scale = .07, mayChange=True
        )

        render.ls()

        self.accept('mouse1', self.mouseOneEvent)
        self.accept('mouse2', self.mouseTwoEvent)
        self.accept('mouse3', self.mouseThreeEvent)

        self.accept('arrow_left', self.keyLeft)
        self.accept('arrow_left-repeat', self.keyLeft)

        self.accept('arrow_right', self.keyRight)
        self.accept('arrow_right-repeat', self.keyRight)

        self.accept('arrow_up', self.keyUp)
        self.accept('arrow_up-repeat', self.keyUp)

        self.accept('arrow_down', self.keyDown)
        self.accept('arrow_down-repeat', self.keyDown)

        self.mapSize = (64,64,16)
        self.viewPosition = (0,0,0)
        self.camOffset = (-32,-32,32)

        self.worldMap = WorldMap(16, self.mapSize, 8, 122011)

        self.activeVoxel = False

        self.mouseBlock = self.loader.loadModel("models/box")
        self.mouseBlock.reparentTo(self.render)

        self.sun = DirectionalLight('sun')

        self.sun.setColor(VBase4(1.0,0.5,0.5,1))
        self.dlnp = render.attachNewNode(self.sun)
        render.setLight(self.dlnp)
        render.setShaderInput("sun", self.dlnp)

        self.pickerRay = CollisionRay()
        self.camLens.setFar(200.0)
        self.taskMgr.add(self.dayCycleTask, "dayCycleTask")
        self.cam.setPos(self.camOffset)
        self.cam.setHpr(-45,-30,0)
        self.taskMgr.add(self.drawLandscapeTask, "drawLandscapeTask")
        self.taskMgr.add(self.mouseCursorTask, "mouseCursorTask")

    def getCamPos(self):
        return (
            self.viewPosition[0] + self.camOffset[0],
            self.viewPosition[1] + self.camOffset[1],
            self.viewPosition[2] + self.camOffset[2]
            )

    def keyLeft(self):
        self.viewPosition = (
            self.viewPosition[0] + (-1 if self.viewPosition[0] > 0 else 0),
            self.viewPosition[1] + (1 if self.viewPosition[1] < self.mapSize[1] else 0),
            self.viewPosition[2]
            )
        self.cam.setPos(self.getCamPos())

    def keyRight(self):
        self.viewPosition = (
            self.viewPosition[0] + (1 if self.viewPosition[0] < self.mapSize[0] else 0),
            self.viewPosition[1] + (-1 if self.viewPosition[1] > 0 else 0),
            self.viewPosition[2]
            )
        self.cam.setPos(self.getCamPos())

    def keyUp(self):
        self.viewPosition = (
            self.viewPosition[0] + (1 if self.viewPosition[0] < self.mapSize[0] else 0),
            self.viewPosition[1] + (1 if self.viewPosition[1] < self.mapSize[0] else 0),
            self.viewPosition[2]
            )
        self.cam.setPos(self.getCamPos())

    def keyDown(self):
        self.viewPosition = (
            self.viewPosition[0] + (-1 if self.viewPosition[0] > 0 else 0),
            self.viewPosition[1] + (-1 if self.viewPosition[1] > 0 else 0),
            self.viewPosition[2]
            )
        self.cam.setPos(self.getCamPos())


    def mouseOneEvent(self):
        if(self.activeVoxel):
            self.worldMap.addVolume(self.activeVoxel)
        self.mouseStat.setText('Left')

    def mouseTwoEvent(self):
        self.mouseStat.setText('Middle')

    def mouseThreeEvent(self):
        if(self.activeVoxel):
            self.worldMap.removeVolume(self.activeVoxel)
        self.mouseStat.setText('Right')

    def drawLandscapeTask(self, task):
        self.worldMap.update(self.viewPosition)
        #self.worldMap.drawNavMesh()
        return task.again

    def mouseCursorTask(self, task):
        if base.mouseWatcherNode.hasMouse():
            mpos = base.mouseWatcherNode.getMouse()
            self.pickerRay.setFromLens(base.camNode, mpos)

            nearPoint = render.getRelativePoint(base.cam, self.pickerRay.getOrigin())
            farPoint = render.getRelativePoint(base.cam, self.pickerRay.getDirection())

            surfacePoint = self.worldMap.bresenham(nearPoint, farPoint)

            self.activeVoxel = surfacePoint

            if(self.activeVoxel):
                self.volume.setText("{0}".format(self.worldMap.getVolume(self.activeVoxel[0], self.activeVoxel[1], self.activeVoxel[2])))
                self.mouseBlock.setPos(
                    surfacePoint[0] - 0.5,
                    surfacePoint[1] - 0.5,
                    surfacePoint[2] + 0.5
                    )

        return task.again

    def dayCycleTask(self, task):
        angleDegrees = task.time * 20.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.dlnp.setPos(16,16,16)
        self.dlnp.setHpr(angleDegrees * 4, 45, 0)

        return task.cont
'''
    oooooo   oooooo     oooo                    oooo        .o8  ooo        ooooo                      
     `888.    `888.     .8'                     `888       "888  `88.       .888'                      
      `888.   .8888.   .8'    .ooooo.  oooo d8b  888   .oooo888   888b     d'888   .oooo.   oo.ooooo.  
       `888  .8'`888. .8'    d88' `88b `888""8P  888  d88' `888   8 Y88. .P  888  `P  )88b   888' `88b 
        `888.8'  `888.8'     888   888  888      888  888   888   8  `888'   888   .oP"888   888   888 
         `888'    `888'      888   888  888      888  888   888   8    Y     888  d8(  888   888   888 
          `8'      `8'       `Y8bod8P' d888b    o888o `Y8bod88P" o8o        o888o `Y888""8o  888bod8P' 
                                                                                             888       
                                                                                            o888o      
'''
class WorldMap:

    def __init__(self, chunkSize, mapSize, seaLevel, seed):
        self.chunkSize = chunkSize
        self.mapSize = mapSize
        self.seaLevel = seaLevel
        self.terrainGen = Terrain(seaLevel,seed)
        self.chunks = {}
        self.materialChunks = {}
        self.geomNodes = {}
        self.travelGrid = numpy.zeros(self.mapSize, int)
        self.travelLines = {}
        self.travelVerts = []   
        self.setupMap()

        self.shader = loader.loadShader('bin/moosecore/marchingcubes.sha')

        self.rock = loader.loadTexture('resources/rock.jpg')
        self.stageRock = TextureStage("Rock")
        self.stageRock.setSort(1)

        self.dirt = loader.loadTexture('resources/dirt.jpg')
        self.stageDirt = TextureStage("Dirt")
        self.stageDirt.setSort(2)

        self.clay = loader.loadTexture('resources/clay.jpg')
        self.stageClay = TextureStage("Clay")
        self.stageClay.setSort(3)

        self.sand = loader.loadTexture('resources/sand.jpg')
        self.stageSand = TextureStage("Sand")
        self.stageSand.setSort(4)

        self.grass = loader.loadTexture('resources/grass.jpg')
        self.stageGrass = TextureStage("Grass")
        self.stageGrass.setSort(5)

    def update(self, playerPos):
        #generate chunks near player
        nodeLoc = (
            (int) (playerPos[0] / self.chunkSize),
            (int) (playerPos[1] / self.chunkSize), 
            (int) (playerPos[2] / self.chunkSize)
            )

        for x in range(-1, 2):
            for y in range(-1, 2):
                if(
                    nodeLoc[0] + x >= 0 
                    and nodeLoc[1] + y >= 0
                    and nodeLoc[0] + x < self.mapSize[0] / self.chunkSize
                    and nodeLoc[1] + y < self.mapSize[1] / self.chunkSize
                    ):
                    self.drawChunk((
                        nodeLoc[0] + x,
                        nodeLoc[1] + y,
                        nodeLoc[2]
                        ))

    def genTravelGrid(self, mapRef):
        if mapRef in self.travelLines:
            self.travelLines[mapRef].reset()
        else:
            self.travelLines[mapRef] = LineNodePath(parent = render, thickness = 3.0, colorVec = Vec4(0, 0, 1, 1))
        
        for x in range(self.chunkSize):
            for y in range(self.chunkSize):
                for z in range(self.chunkSize):
                    self.walkNodes(mapRef, (x,y,z))

        self.travelLines[mapRef].drawLines(self.travelVerts)
        self.travelLines[mapRef].create()

    def nodeWalkable(self, position):
        if self.getPoint(position[0], position[1], position[2]) and not self.getPoint(position[0], position[1], position[2] + 1) and not self.getPoint(position[0], position[1], position[2] + 2):
            return True
        else:
            return False

    def walkNodes(self, mapRef, offset):
        position = (
            mapRef[0] * self.chunkSize + offset[0],
            mapRef[1] * self.chunkSize + offset[1],
            mapRef[2] * self.chunkSize + offset[2]
            )

        self.travelGrid[position] = 0

        if self.nodeWalkable(position):
            if self.nodeWalkable((position[0], position[1] + 1, position[2])):
                self.travelGrid[position] |= 1
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0], position[1] + 1, position[2] + 1)))

            if self.nodeWalkable((position[0] + 1, position[1] + 1, position[2])):
                self.travelGrid[position] |= 2
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] + 1, position[1] + 1, position[2] + 1)))

            if self.nodeWalkable((position[0] + 1, position[1], position[2])):
                self.travelGrid[position] |= 4
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] + 1, position[1], position[2] + 1)))

            if self.nodeWalkable((position[0] + 1, position[1] - 1, position[2])):
                self.travelGrid[position] |= 8
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] + 1, position[1] - 1, position[2] + 1)))

            if self.nodeWalkable((position[0], position[1] - 1, position[2])):
                self.travelGrid[position] |= 16
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0], position[1] - 1, position[2] + 1)))

            if self.nodeWalkable((position[0] - 1, position[1] - 1, position[2])):
                self.travelGrid[position] |= 32
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] - 1, position[1] - 1, position[2] + 1)))

            if self.nodeWalkable((position[0] - 1, position[1], position[2])):
                self.travelGrid[position] |= 64
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] - 1, position[1], position[2] + 1)))

            if self.nodeWalkable((position[0] - 1, position[1] + 1, position[2])):
                self.travelGrid[position] |= 128
                #self.travelVerts.append(((position[0], position[1], position[2] +1), (position[0] - 1, position[1] + 1, position[2] + 1)))

    def addVolume(self, position):
        mapRef = (position[0] / self.chunkSize, position[1] / self.chunkSize, position[2] / self.chunkSize)

        if mapRef not in self.chunks:
            return False

        if self.chunks[mapRef][position[0] % self.chunkSize, position[1] % self.chunkSize, position[2] % self.chunkSize] < 1:
            self.chunks[mapRef][position[0] % self.chunkSize, position[1] % self.chunkSize, position[2] % self.chunkSize] = 1
            self.updateChunk(mapRef)
            self.genTravelGrid(mapRef)
        else:
            self.addVolume((position[0], position[1], position[2] + 1))

    def removeVolume(self, position):
        mapRef = (position[0] / self.chunkSize, position[1] / self.chunkSize, position[2] / self.chunkSize)

        if mapRef not in self.chunks:
            return False

        if self.chunks[mapRef][position[0] % self.chunkSize, position[1] % self.chunkSize, position[2] % self.chunkSize] > 0:
            self.chunks[mapRef][position[0] % self.chunkSize, position[1] % self.chunkSize, position[2] % self.chunkSize] = 0
            self.updateChunk(mapRef)
            self.genTravelGrid(mapRef)
        else:
            self.removeVolume((position[0], position[1], position[2] - 1))

    def drawChunk(self, location):
        if location not in self.chunks:
            self.loadChunk(location)

        if location not in self.geomNodes:
            self.updateChunk(location)

    def updateChunk(self, location):
        if location not in self.chunks:
            self.loadChunk(location)

        newMesh = marchingCubes.MarchingCubes.generate(
            self, 
            (self.chunkSize, self.chunkSize, self.chunkSize),
            (location[0] * self.chunkSize, location[1] * self.chunkSize, location[2] * self.chunkSize)
        )
        if location in self.geomNodes:
            self.geomNodes[location].removeNode()        

        self.geomNodes[location] = render.attachNewNode(newMesh)
        self.geomNodes[location].setShader(self.shader)
        self.geomNodes[location].setTexture(self.stageClay, self.clay)
        self.geomNodes[location].setTexture(self.stageDirt, self.dirt)
        self.geomNodes[location].setTexture(self.stageSand, self.sand)
        self.geomNodes[location].setTexture(self.stageGrass, self.grass)
        self.geomNodes[location].setTexture(self.stageRock, self.rock)
        self.geomNodes[location].setTwoSided(False)

    def getVolume(self, x_pos, y_pos, z_pos):
        mapRef = (x_pos / self.chunkSize, y_pos / self.chunkSize, z_pos / self.chunkSize)

        if mapRef not in self.chunks:
            return False

        return self.chunks[mapRef][x_pos % self.chunkSize, y_pos % self.chunkSize, z_pos % self.chunkSize]

    def getPoint(self, x_pos, y_pos, z_pos):
        mapRef = (x_pos / self.chunkSize, y_pos / self.chunkSize, z_pos / self.chunkSize)

        if mapRef not in self.chunks:
            return False

        if self.chunks[mapRef][x_pos % self.chunkSize, y_pos % self.chunkSize, z_pos % self.chunkSize] > 0:
            return True
        else:
            return False

    def bresenham(self, startPoint, endPoint):
        startPoint = [int(startPoint[0]),int(startPoint[1]),int(startPoint[2])]

        endPoint = [int(endPoint[0]),int(endPoint[1]),int(endPoint[2])]

        steepXY = (abs(endPoint[1] - startPoint[1]) > abs(endPoint[0] - startPoint[0]))
        if(steepXY):   
            startPoint[0], startPoint[1] = startPoint[1], startPoint[0]
            endPoint[0], endPoint[1] = endPoint[1], endPoint[0]

        steepXZ = (abs(endPoint[2] - startPoint[2]) > abs(endPoint[0] - startPoint[0]))
        if(steepXZ):
            startPoint[0], startPoint[2] = startPoint[2], startPoint[0]
            endPoint[0], endPoint[2] = endPoint[2], endPoint[0]

        delta = [abs(endPoint[0] - startPoint[0]), abs(endPoint[1] - startPoint[1]), abs(endPoint[2] - startPoint[2])]

        errorXY = delta[0] / 2
        errorXZ = delta[0] / 2

        step = [
            -1 if startPoint[0] > endPoint[0] else 1,
            -1 if startPoint[1] > endPoint[1] else 1,
            -1 if startPoint[2] > endPoint[2] else 1
        ]

        y = startPoint[1]
        z = startPoint[2]

        for x in range(startPoint[0], endPoint[0], step[0]):
            point = [x, y, z]

            if(steepXZ):
                point[0], point[2] = point[2], point[0]
            if(steepXY):
                point[0], point[1] = point[1], point[0]

            if(self.getPoint(point[0], point[1], point[2])):
                return (point[0], point[1], point[2])

            errorXY -= delta[1]
            errorXZ -= delta[2]

            if(errorXY < 0):
                y += step[1]
                errorXY += delta[0]

            if(errorXZ < 0):
                z += step[2]
                errorXZ += delta[0]

        return False


    def getCacheGroup(self, x_pos, y_pos, z_pos):
        mapRef = (x_pos/self.chunkSize, y_pos/self.chunkSize, z_pos/self.chunkSize)

        if mapRef not in self.chunks:
            self.loadChunk(mapRef)

        #This order is stupid; fixing requires regenerating Magic Numbers :(
        return [
            self.getCachePoint(x_pos + 0, y_pos + 0, z_pos + 0),
            self.getCachePoint(x_pos + 1, y_pos + 0, z_pos + 0),
            self.getCachePoint(x_pos + 1, y_pos + 0, z_pos + 1),
            self.getCachePoint(x_pos + 0, y_pos + 0, z_pos + 1),
            self.getCachePoint(x_pos + 0, y_pos + 1, z_pos + 0),
            self.getCachePoint(x_pos + 1, y_pos + 1, z_pos + 0),
            self.getCachePoint(x_pos + 1, y_pos + 1, z_pos + 1),
            self.getCachePoint(x_pos + 0, y_pos + 1, z_pos + 1)
        ]
    
    def getCachePoint(self, x_pos, y_pos, z_pos):
        mapRef = (x_pos/self.chunkSize, y_pos/self.chunkSize, z_pos/self.chunkSize)

        if mapRef not in self.chunks:
            self.loadChunk(mapRef)

        return (
            self.chunks[mapRef][x_pos % self.chunkSize, y_pos % self.chunkSize, z_pos % self.chunkSize], 
            self.materialChunks[mapRef][x_pos % self.chunkSize, y_pos % self.chunkSize, z_pos % self.chunkSize]
            )

    def setupMap(self):
        for x in range(self.mapSize[0] / self.chunkSize):
            for y in range(self.mapSize[1] / self.chunkSize):
                for z in range(self.mapSize[2] / self.chunkSize):
                    mapRef = (x,y,z)
                    self.loadChunk(mapRef)
        self.genTravelGrid(mapRef)

    def loadChunk(self, mapRef):
        self.generateChunk(mapRef)

    def generateChunk(self, mapRef):
        self.chunks[mapRef] = numpy.zeros((self.chunkSize, self.chunkSize, self.chunkSize), float)
        self.materialChunks[mapRef] = numpy.zeros((self.chunkSize, self.chunkSize, self.chunkSize), int)

        for x in range(self.chunkSize):
            for y in range(self.chunkSize):
                for z in range(self.chunkSize):
                    point = (
                        (mapRef[0] * self.chunkSize) + x,
                        (mapRef[1] * self.chunkSize) + y,
                        (mapRef[2] * self.chunkSize) + z
                        )

                    self.chunks[mapRef][x,y,z], self.materialChunks[mapRef][x,y,z] = self.terrainGen.getGeneratedPoint(point)

'''
    ooooooooooooo                                        o8o              
    8'   888   `8                                        `"'              
         888       .ooooo.  oooo d8b oooo d8b  .oooo.   oooo  ooo. .oo.   
         888      d88' `88b `888""8P `888""8P `P  )88b  `888  `888P"Y88b  
         888      888ooo888  888      888      .oP"888   888   888   888  
         888      888    .o  888      888     d8(  888   888   888   888  
        o888o     `Y8bod8P' d888b    d888b    `Y888""8o o888o o888o o888o 
'''                                                                       
class Terrain:

    def __init__(self, seaLevel, seed):
        self.seaLevel = seaLevel
        self.heightMap = PerlinNoise2(16.0,16.0, 256, seed)
        self.noise = PerlinNoise3(4.0,4.0,16.0, 256, seed)

    def loadTerrain(self):
        program = [
            
        ]
        pass

    def getGeneratedPoint(self, point):
        density =  0-cmp(point[2], self.seaLevel)

        material = self.noise(point[0] / 5.0, point[1] / 5.0, point[2] / 5.0)

        density-=  self.noise(point[0] / 2.0, point[1] / 2.0, point[2] / 2.0) * 2
        
        density-=  self.noise(point[0] / 2.0, point[1] / 2.0, point[2] / 2.0) * 2

        density-=  self.noise(point[0] / 1.5, point[1] / 0.5, point[2] / 0.5) * 2

        density-= (float(point[2]) / 2) / self.seaLevel
        #if(density > 0): density = 0

        if(density < -1): density = -1
        if(density > 1): density = 1

        if material > 0.5:
            material = 3
        elif material > 0:
            material = 2
        elif material > -0.5:
            material = 1
        else:
            material = 0

        return round(density, 1), material

'''
    oooooo     oooo                                 oooo      
     `888.     .8'                                  `888      
      `888.   .8'    .ooooo.  oooo    ooo  .ooooo.   888      
       `888. .8'    d88' `88b  `88b..8P'  d88' `88b  888      
        `888.8'     888   888    Y888'    888ooo888  888  o8o 
         `888'      888   888  .o8"'88b   888    .o  888  `"' 
          `8'       `Y8bod8P' o88'   888o `Y8bod8P' o888o o8o 
                                                          `"' 
'''                                                       
class Voxel:
    vertexMap = [
        [[ 1, 1,-1],[-1, 1,-1],[-1, 1, 1],[ 1, 1, 1]],
        [[ 1,-1, 1],[-1,-1, 1],[-1,-1,-1],[ 1,-1,-1]],
        [[-1,-1, 1],[ 1,-1, 1],[ 1, 1, 1],[-1, 1, 1]],
        [[ 1,-1,-1],[-1,-1,-1],[-1, 1,-1],[ 1, 1,-1]],
        [[-1,-1,-1],[-1,-1, 1],[-1, 1, 1],[-1, 1,-1]],
        [[ 1,-1, 1],[ 1,-1,-1],[ 1, 1,-1],[ 1, 1, 1]]
    ]

    faceNormals = [[ 0, 1, 0],[ 0,-1, 0],[ 0, 0, 1],[ 0, 0,-1],[ 1, 0, 0],[-1, 0, 0]]

    texCoords = [
        [0.0,1.0],
        [0.0,0.0],
        [1.0,0.0],
        [1.0,1.0]
    ]

    cubeColors = [
        [0.5,0.0,0.0],
        [0.0,0.0,0.5],
        [1.0,0.0,0.5],
        [0.5,0.0,1.0],
        [0.0,0.5,0.0],
        [1.0,0.5,0.0],
        [0.5,1.0,0.0],
        [0.0,0.5,1.0],
        [0.0,1.0,0.5],
        [1.0,0.5,1.0],
        [1.0,1.0,0.5],
        [0.5,1.0,1.0]
    ]

    def __init__(self):
        faces = (0,1,2,3,4,5)

        worldSize = (64,64,16)

        self.world = Terrain()
        Voxel.marchingCube(self.world, worldSize,0.5)
        #Voxel.buildChunk(lemmingData, Voxel.pointSprite,0.5)
        #Voxel.buildChunk(lemmingData, Voxel.culledCube, 0.5)


     
    @staticmethod
    def buildChunk(chunkData, renderMode, scale):
        faces = (0,1,2,3,4,5)
        for z, y_row in enumerate(chunkData):
            for y, x_row in enumerate(y_row):
                for x, data in enumerate(x_row):
                    if data is not 0:
                        renderMode(
                            (0 - ((len(x_row)-1)/2.0) + x) * scale, 
                            (0 - ((len(y_row)-1)/2.0) + y) * scale, 
                            (0 - ((len(chunkData)-1)/2.0) + z) * scale, 
                            scale, faces, data
                        )

    @staticmethod
    def color8to32(inColor):
        r = (inColor >> 6)
        g = (inColor >> 4) & 3
        b = (inColor >> 2) & 3
        a = inColor & 3

        return (r * 85 / 255.0, g * 85 / 255.0, b * 85 / 255.0, a * 85 / 255.0)
    
    '''
    ooooooooo.              o8o                  .       .oooooo..o                      o8o      .                      
    `888   `Y88.            `"'                .o8      d8P'    `Y8                      `"'    .o8                      
     888   .d88'  .ooooo.  oooo  ooo. .oo.   .o888oo    Y88bo.      oo.ooooo.  oooo d8b oooo  .o888oo  .ooooo.   .oooo.o 
     888ooo88P'  d88' `88b `888  `888P"Y88b    888       `"Y8888o.   888' `88b `888""8P `888    888   d88' `88b d88(  "8 
     888         888   888  888   888   888    888           `"Y88b  888   888  888      888    888   888ooo888 `"Y88b.  
     888         888   888  888   888   888    888 .    oo     .d8P  888   888  888      888    888 . 888    .o o.  )88b 
    o888o        `Y8bod8P' o888o o888o o888o   "888"    8""88888P'   888bod8P' d888b    o888o   "888" `Y8bod8P' 8""888P' 
                                                                     888                                                 
                                                                    o888o                                                
    '''                                                                                                                      
    @staticmethod
    def pointSprite(center_x, center_y, center_z, size, faces, data):
        snode = GeomNode('point')
        if len(faces) > 0:
            format = GeomVertexFormat.getV3cp()
            vdata = GeomVertexData('point', format, Geom.UHDynamic)

            colorData = Voxel.color8to32(data)

            vertex = GeomVertexWriter(vdata, 'vertex')
            color = GeomVertexWriter(vdata, 'color')

            vertex.addData3f(center_x, center_y, center_z)
            color.addData4f(colorData[0], colorData[1], colorData[2], colorData[3])

            point = GeomPoints(Geom.UHDynamic)

            point.addVertex(0)

            point.closePrimitive()

            pointCloud = Geom(vdata)

            pointCloud.addPrimitive(point)

            snode.addGeom(pointCloud)
            
            pc = render.attachNewNode(snode)

            pc.setRenderModeThickness((size / 2.0) * 1.5)
            pc.setRenderModePerspective(True)
    '''
      .oooooo.               oooo  oooo                  .o8       .oooooo.                .o8                          
     d8P'  `Y8b              `888  `888                 "888      d8P'  `Y8b              "888                          
    888          oooo  oooo   888   888   .ooooo.   .oooo888     888          oooo  oooo   888oooo.   .ooooo.   .oooo.o 
    888          `888  `888   888   888  d88' `88b d88' `888     888          `888  `888   d88' `88b d88' `88b d88(  "8 
    888           888   888   888   888  888ooo888 888   888     888           888   888   888   888 888ooo888 `"Y88b.  
    `88b    ooo   888   888   888   888  888    .o 888   888     `88b    ooo   888   888   888   888 888    .o o.  )88b 
     `Y8bood8P'   `V88V"V8P' o888o o888o `Y8bod8P' `Y8bod88P"     `Y8bood8P'   `V88V"V8P'  `Y8bod8P' `Y8bod8P' 8""888P' 
                                                                                                                        
                                                                                                                        
    '''                                                                                                                        
    @staticmethod
    def culledCube(center_x, center_y, center_z, size, faces, data):
        snode = GeomNode('cube')
        for face in faces:
            snode.addGeom(Voxel.square(face, center_x, center_y, center_z, size, data))

        cube = render.attachNewNode(snode)

        #FIXME I don't like this - I want a way to manage textures seperately to DynGeom.
        texture = loader.loadTexture('resources/texturepack.png')
        texture.setMagfilter(Texture.FTNearest)
        texture.setMinfilter(Texture.FTLinear)

        cube.setTexture(texture)

        cube.setTwoSided(True)

    @staticmethod
    def square(face, center_x, center_y, center_z, size, data):
        format=GeomVertexFormat.getV3n3cpt2()
        vdata=GeomVertexData('square', format, Geom.UHDynamic)

        colorData = (1.0,1.0,1.0,1.0)

        block_x = ((data) / 16) * 0.0625
        block_y = ((data) % 16) * 0.0625

        vertex=GeomVertexWriter(vdata, 'vertex')
        normal=GeomVertexWriter(vdata, 'normal')
        color=GeomVertexWriter(vdata, 'color')
        texcoord=GeomVertexWriter(vdata, 'texcoord')

        for vert in range(4):
            vertex.addData3f(
                center_x + Voxel.vertexMap[face][vert][0] * (size / 2.0),
                center_y + Voxel.vertexMap[face][vert][1] * (size / 2.0),
                center_z + Voxel.vertexMap[face][vert][2] * (size / 2.0)
                )
            normal.addData3f(
                Voxel.faceNormals[face][0],
                Voxel.faceNormals[face][1],
                Voxel.faceNormals[face][2],
                )

            color.addData4f(
                colorData[0],colorData[1],colorData[2],colorData[3]
                )

            texcoord.addData2f(
                Voxel.texCoords[vert][0] * 0.0625 + block_x,
                Voxel.texCoords[vert][1] * 0.0625 + block_y
                )

        tri1 = GeomTriangles(Geom.UHDynamic)
        tri2 = GeomTriangles(Geom.UHDynamic)

        tri1.addVertex(0)
        tri1.addVertex(1)
        tri1.addVertex(3)

        tri2.addVertex(1)
        tri2.addVertex(2)
        tri2.addVertex(3)

        tri1.closePrimitive()
        tri2.closePrimitive()

        square=Geom(vdata)
        square.addPrimitive(tri1)
        square.addPrimitive(tri2)

        return square


def main():
    canton = Canton()
    
    canton.run()

if __name__ == '__main__':
    main()