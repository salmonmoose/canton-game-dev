from math import pi, sin, cos

from direct.showbase.ShowBase import ShowBase
from direct.gui.DirectGui import *
from direct.task import Task
from panda3d.core import *

import numpy

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

cubeTris = [
    [],
    [0,8,3],
    [0,1,9],
    [1,8,3,9,8,1],
    [1,2,10],
    [0,8,3,1,2,10],
    [9,2,10,0,2,9],
    [2,8,3,2,10,8,10,9,8],
    [3,11,2],
    [0,11,2,8,11,0],
    [1,9,0,2,3,11],
    [1,11,2,1,9,11,9,8,11],
    [3,10,1,11,10,3],
    [0,10,1,0,8,10,8,11,10],
    [3,9,0,3,11,9,11,10,9],
    [9,8,10,10,8,11],
    [4,7,8],
    [4,3,0,7,3,4],
    [0,1,9,8,4,7],
    [4,1,9,4,7,1,7,3,1],
    [1,2,10,8,4,7],
    [3,4,7,3,0,4,1,2,10],
    [9,2,10,9,0,2,8,4,7],
    [2,10,9,2,9,7,2,7,3,7,9,4],
    [8,4,7,3,11,2],
    [11,4,7,11,2,4,2,0,4],
    [9,0,1,8,4,7,2,3,11],
    [4,7,11,9,4,11,9,11,2,9,2,1],
    [3,10,1,3,11,10,7,8,4],
    [1,11,10,1,4,11,1,0,4,7,11,4],
    [4,7,8,9,0,11,9,11,10,11,0,3],
    [4,7,11,4,11,9,9,11,10],
    [9,5,4],
    [9,5,4,0,8,3],
    [0,5,4,1,5,0],
    [8,5,4,8,3,5,3,1,5],
    [1,2,10,9,5,4],
    [3,0,8,1,2,10,4,9,5],
    [5,2,10,5,4,2,4,0,2],
    [2,10,5,3,2,5,3,5,4,3,4,8],
    [9,5,4,2,3,11],
    [0,11,2,0,8,11,4,9,5],
    [0,5,4,0,1,5,2,3,11],
    [2,1,5,2,5,8,2,8,11,4,8,5],
    [10,3,11,10,1,3,9,5,4],
    [4,9,5,0,8,1,8,10,1,8,11,10],
    [5,4,0,5,0,11,5,11,10,11,0,3],
    [5,4,8,5,8,10,10,8,11],
    [9,7,8,5,7,9],
    [9,3,0,9,5,3,5,7,3],
    [0,7,8,0,1,7,1,5,7],
    [1,5,3,3,5,7],
    [9,7,8,9,5,7,10,1,2],
    [10,1,2,9,5,0,5,3,0,5,7,3],
    [8,0,2,8,2,5,8,5,7,10,5,2],
    [2,10,5,2,5,3,3,5,7],
    [7,9,5,7,8,9,3,11,2],
    [9,5,7,9,7,2,9,2,0,2,7,11],
    [2,3,11,0,1,8,1,7,8,1,5,7],
    [11,2,1,11,1,7,7,1,5],
    [9,5,8,8,5,7,10,1,3,10,3,11],
    [5,7,0,5,0,9,7,11,0,1,0,10,11,10,0],
    [11,10,0,11,0,3,10,5,0,8,0,7,5,7,0],
    [11,10,5,7,11,5],
    [10,6,5],
    [0,8,3,5,10,6],
    [9,0,1,5,10,6],
    [1,8,3,1,9,8,5,10,6],
    [1,6,5,2,6,1],
    [1,6,5,1,2,6,3,0,8],
    [9,6,5,9,0,6,0,2,6],
    [5,9,8,5,8,2,5,2,6,3,2,8],
    [2,3,11,10,6,5],
    [11,0,8,11,2,0,10,6,5],
    [0,1,9,2,3,11,5,10,6],
    [5,10,6,1,9,2,9,11,2,9,8,11],
    [6,3,11,6,5,3,5,1,3],
    [0,8,11,0,11,5,0,5,1,5,11,6],
    [3,11,6,0,3,6,0,6,5,0,5,9],
    [6,5,9,6,9,11,11,9,8],
    [5,10,6,4,7,8],
    [4,3,0,4,7,3,6,5,10],
    [1,9,0,5,10,6,8,4,7],
    [10,6,5,1,9,7,1,7,3,7,9,4],
    [6,1,2,6,5,1,4,7,8],
    [1,2,5,5,2,6,3,0,4,3,4,7],
    [8,4,7,9,0,5,0,6,5,0,2,6],
    [7,3,9,7,9,4,3,2,9,5,9,6,2,6,9],
    [3,11,2,7,8,4,10,6,5],
    [5,10,6,4,7,2,4,2,0,2,7,11],
    [0,1,9,4,7,8,2,3,11,5,10,6],
    [9,2,1,9,11,2,9,4,11,7,11,4,5,10,6],
    [8,4,7,3,11,5,3,5,1,5,11,6],
    [5,1,11,5,11,6,1,0,11,7,11,4,0,4,11],
    [0,5,9,0,6,5,0,3,6,11,6,3,8,4,7],
    [6,5,9,6,9,11,4,7,9,7,11,9],
    [10,4,9,6,4,10],
    [4,10,6,4,9,10,0,8,3],
    [10,0,1,10,6,0,6,4,0],
    [8,3,1,8,1,6,8,6,4,6,1,10],
    [1,4,9,1,2,4,2,6,4],
    [3,0,8,1,2,9,2,4,9,2,6,4],
    [0,2,4,4,2,6],
    [8,3,2,8,2,4,4,2,6],
    [10,4,9,10,6,4,11,2,3],
    [0,8,2,2,8,11,4,9,10,4,10,6],
    [3,11,2,0,1,6,0,6,4,6,1,10],
    [6,4,1,6,1,10,4,8,1,2,1,11,8,11,1],
    [9,6,4,9,3,6,9,1,3,11,6,3],
    [8,11,1,8,1,0,11,6,1,9,1,4,6,4,1],
    [3,11,6,3,6,0,0,6,4],
    [6,4,8,11,6,8],
    [7,10,6,7,8,10,8,9,10],
    [0,7,3,0,10,7,0,9,10,6,7,10],
    [10,6,7,1,10,7,1,7,8,1,8,0],
    [10,6,7,10,7,1,1,7,3],
    [1,2,6,1,6,8,1,8,9,8,6,7],
    [2,6,9,2,9,1,6,7,9,0,9,3,7,3,9],
    [7,8,0,7,0,6,6,0,2],
    [7,3,2,6,7,2],
    [2,3,11,10,6,8,10,8,9,8,6,7],
    [2,0,7,2,7,11,0,9,7,6,7,10,9,10,7],
    [1,8,0,1,7,8,1,10,7,6,7,10,2,3,11],
    [11,2,1,11,1,7,10,6,1,6,7,1],
    [8,9,6,8,6,7,9,1,6,11,6,3,1,3,6],
    [0,9,1,11,6,7],
    [7,8,0,7,0,6,3,11,0,11,6,0],
    [7,11,6],
    [7,6,11],
    [3,0,8,11,7,6],
    [0,1,9,11,7,6],
    [8,1,9,8,3,1,11,7,6],
    [10,1,2,6,11,7],
    [1,2,10,3,0,8,6,11,7],
    [2,9,0,2,10,9,6,11,7],
    [6,11,7,2,10,3,10,8,3,10,9,8],
    [7,2,3,6,2,7],
    [7,0,8,7,6,0,6,2,0],
    [2,7,6,2,3,7,0,1,9],
    [1,6,2,1,8,6,1,9,8,8,7,6],
    [10,7,6,10,1,7,1,3,7],
    [10,7,6,1,7,10,1,8,7,1,0,8],
    [0,3,7,0,7,10,0,10,9,6,10,7],
    [7,6,10,7,10,8,8,10,9],
    [6,8,4,11,8,6],
    [3,6,11,3,0,6,0,4,6],
    [8,6,11,8,4,6,9,0,1],
    [9,4,6,9,6,3,9,3,1,11,3,6],
    [6,8,4,6,11,8,2,10,1],
    [1,2,10,3,0,11,0,6,11,0,4,6],
    [4,11,8,4,6,11,0,2,9,2,10,9],
    [10,9,3,10,3,2,9,4,3,11,3,6,4,6,3],
    [8,2,3,8,4,2,4,6,2],
    [0,4,2,4,6,2],
    [1,9,0,2,3,4,2,4,6,4,3,8],
    [1,9,4,1,4,2,2,4,6],
    [8,1,3,8,6,1,8,4,6,6,10,1],
    [10,1,0,10,0,6,6,0,4],
    [4,6,3,4,3,8,6,10,3,0,3,9,10,9,3],
    [10,9,4,6,10,4],
    [4,9,5,7,6,11],
    [0,8,3,4,9,5,11,7,6],
    [5,0,1,5,4,0,7,6,11],
    [11,7,6,8,3,4,3,5,4,3,1,5],
    [9,5,4,10,1,2,7,6,11],
    [6,11,7,1,2,10,0,8,3,4,9,5],
    [7,6,11,5,4,10,4,2,10,4,0,2],
    [3,4,8,3,5,4,3,2,5,10,5,2,11,7,6],
    [7,2,3,7,6,2,5,4,9],
    [9,5,4,0,8,6,0,6,2,6,8,7],
    [3,6,2,3,7,6,1,5,0,5,4,0],
    [6,2,8,6,8,7,2,1,8,4,8,5,1,5,8],
    [9,5,4,10,1,6,1,7,6,1,3,7],
    [1,6,10,1,7,6,1,0,7,8,7,0,9,5,4],
    [4,0,10,4,10,5,0,3,10,6,10,7,3,7,10],
    [7,6,10,7,10,8,5,4,10,4,8,10],
    [6,9,5,6,11,9,11,8,9],
    [3,6,11,0,6,3,0,5,6,0,9,5],
    [0,11,8,0,5,11,0,1,5,5,6,11],
    [6,11,3,6,3,5,5,3,1],
    [1,2,10,9,5,11,9,11,8,11,5,6],
    [0,11,3,0,6,11,0,9,6,5,6,9,1,2,10],
    [11,8,5,11,5,6,8,0,5,10,5,2,0,2,5],
    [6,11,3,6,3,5,2,10,3,10,5,3],
    [5,8,9,5,2,8,5,6,2,3,8,2],
    [9,5,6,9,6,0,0,6,2],
    [1,5,8,1,8,0,5,6,8,3,8,2,6,2,8],
    [1,5,6,2,1,6],
    [1,3,6,1,6,10,3,8,6,5,6,9,8,9,6],
    [10,1,0,10,0,6,9,5,0,5,6,0],
    [0,3,8,5,6,10],
    [10,5,6],
    [11,5,10,7,5,11],
    [11,5,10,11,7,5,8,3,0],
    [5,11,7,5,10,11,1,9,0],
    [10,7,5,10,11,7,9,8,1,8,3,1],
    [11,1,2,11,7,1,7,5,1],
    [0,8,3,1,2,7,1,7,5,7,2,11],
    [9,7,5,9,2,7,9,0,2,2,11,7],
    [7,5,2,7,2,11,5,9,2,3,2,8,9,8,2],
    [2,5,10,2,3,5,3,7,5],
    [8,2,0,8,5,2,8,7,5,10,2,5],
    [9,0,1,5,10,3,5,3,7,3,10,2],
    [9,8,2,9,2,1,8,7,2,10,2,5,7,5,2],
    [1,3,5,3,7,5],
    [0,8,7,0,7,1,1,7,5],
    [9,0,3,9,3,5,5,3,7],
    [9,8,7,5,9,7],
    [5,8,4,5,10,8,10,11,8],
    [5,0,4,5,11,0,5,10,11,11,3,0],
    [0,1,9,8,4,10,8,10,11,10,4,5],
    [10,11,4,10,4,5,11,3,4,9,4,1,3,1,4],
    [2,5,1,2,8,5,2,11,8,4,5,8],
    [0,4,11,0,11,3,4,5,11,2,11,1,5,1,11],
    [0,2,5,0,5,9,2,11,5,4,5,8,11,8,5],
    [9,4,5,2,11,3],
    [2,5,10,3,5,2,3,4,5,3,8,4],
    [5,10,2,5,2,4,4,2,0],
    [3,10,2,3,5,10,3,8,5,4,5,8,0,1,9],
    [5,10,2,5,2,4,1,9,2,9,4,2],
    [8,4,5,8,5,3,3,5,1],
    [0,4,5,1,0,5],
    [8,4,5,8,5,3,9,0,5,0,3,5],
    [9,4,5],
    [4,11,7,4,9,11,9,10,11],
    [0,8,3,4,9,7,9,11,7,9,10,11],
    [1,10,11,1,11,4,1,4,0,7,4,11],
    [3,1,4,3,4,8,1,10,4,7,4,11,10,11,4],
    [4,11,7,9,11,4,9,2,11,9,1,2],
    [9,7,4,9,11,7,9,1,11,2,11,1,0,8,3],
    [11,7,4,11,4,2,2,4,0],
    [11,7,4,11,4,2,8,3,4,3,2,4],
    [2,9,10,2,7,9,2,3,7,7,4,9],
    [9,10,7,9,7,4,10,2,7,8,7,0,2,0,7],
    [3,7,10,3,10,2,7,4,10,1,10,0,4,0,10],
    [1,10,2,8,7,4],
    [4,9,1,4,1,7,7,1,3],
    [4,9,1,4,1,7,0,8,1,8,7,1],
    [4,0,3,7,4,3],
    [4,8,7],
    [9,10,8,10,11,8],
    [3,0,9,3,9,11,11,9,10],
    [0,1,10,0,10,8,8,10,11],
    [3,1,10,11,3,10],
    [1,2,11,1,11,9,9,11,8],
    [3,0,9,3,9,11,1,2,9,2,11,9],
    [0,2,11,8,0,11],
    [3,2,11],
    [2,3,8,2,8,10,10,8,9],
    [9,10,2,0,9,2],
    [2,3,8,2,8,10,0,1,8,1,10,8],
    [1,10,2],
    [1,3,8,9,1,8],
    [0,9,1],
    [0,3,8],
    []
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

        

        self.title = OnscreenText(
            text='Canton indev',
            style=1, fg=(1,1,1,1), pos=(0.5, -0.95), scale = .07)

        render.ls()

        base.useDrive()
        self.worldMap = WorldMap(16,8,122011)

        self.sun = DirectionalLight('sun')

        self.sun.setColor(VBase4(1.0,0.5,0.5,1))
        self.dlnp = render.attachNewNode(self.sun)
        render.setLight(self.dlnp)
        render.setShaderInput("sun", self.dlnp)

        self.ambient = AmbientLight('amient')
        self.ambient.setColor(VBase4(0.0,0.0,0.5,1))
        self.alnp = render.attachNewNode(self.ambient)
        render.setLight(self.alnp)
        render.setShaderInput("ambient", self.alnp)

        self.camera.setPos(0,0,18)
        self.taskMgr.add(self.spinCameraTask, "spinCameraTask")
        self.taskMgr.add(self.drawLandscapeTask, "drawLandscapeTask")

    def drawLandscapeTask(self, task):
        playerPos = (0.0,0.0,0.0)
        self.worldMap.update(playerPos)


    def spinCameraTask(self, task):
        angleDegrees = task.time * 20.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(64 * sin(angleRadians), -64 * cos(angleRadians), 32)
        self.dlnp.setPos(16,16,16)
        self.dlnp.setHpr(angleDegrees * 4, 45, 0)
        self.camera.setHpr(angleDegrees, -20, 0)

        return Task.cont
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

    def __init__(self, chunkSize, seaLevel, seed):
        self.chunkSize = chunkSize
        self.seaLevel = seaLevel
        self.terrainGen = Terrain(chunkSize,seaLevel,seed)
        self.chunks = {}
        self.geomNodes = {}
        self.shader = loader.loadShader('bin/moosecore/marchingcubes.sha')
        self.rock = loader.loadTexture('resources/rock.jpg')
        self.stageRock = TextureStage("Rock")
        self.stageRock.setSort(1)
        self.grass = loader.loadTexture('resources/grass.jpg')
        self.stageGrass = TextureStage("Grass")
        self.stageGrass.setSort(2)

    def update(self, playerPos):
        #generate chunks near player
        nodeLoc = (
            (int) (playerPos[0] / self.chunkSize),
            (int) (playerPos[1] / self.chunkSize), 
            (int) (playerPos[2] / self.chunkSize)
            )

        for x in range(-1, 2):
            for y in range(-1, 2):
                self.updateChunk((
                    nodeLoc[0] + x,
                    nodeLoc[1] + y,
                    nodeLoc[2]
                    ))

    def updateChunk(self, location):
        if location not in self.chunks:
            self.loadChunk(location)
        self.geomNodes[location] = render.attachNewNode(Voxel.marchingCubes(
            self, 
            (self.chunkSize, self.chunkSize, self.chunkSize),
            (location[0] * self.chunkSize, location[1] * self.chunkSize, location[2] * self.chunkSize),
            0.5)
        )
        self.geomNodes[location].setShader(self.shader)
        self.geomNodes[location].setTexture(self.stageGrass, self.grass)
        self.geomNodes[location].setTexture(self.stageRock, self.rock)
        self.geomNodes[location].setTwoSided(False)


    def getCacheGroup(self, x_pos, y_pos, z_pos):
        mapRef = (x_pos/self.chunkSize, y_pos/self.chunkSize, z_pos/self.chunkSize)

        if mapRef not in self.chunks:
            self.loadChunk(mapRef)

        #This order is stupid fixing requires regenerating Magic Numbers :(
        return [
            self.chunks[mapRef][(x_pos % self.chunkSize) + 0, (y_pos % self.chunkSize) + 0, (z_pos % self.chunkSize) + 0],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 1, (y_pos % self.chunkSize) + 0, (z_pos % self.chunkSize) + 0],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 1, (y_pos % self.chunkSize) + 0, (z_pos % self.chunkSize) + 1],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 0, (y_pos % self.chunkSize) + 0, (z_pos % self.chunkSize) + 1],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 0, (y_pos % self.chunkSize) + 1, (z_pos % self.chunkSize) + 0],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 1, (y_pos % self.chunkSize) + 1, (z_pos % self.chunkSize) + 0],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 1, (y_pos % self.chunkSize) + 1, (z_pos % self.chunkSize) + 1],
            self.chunks[mapRef][(x_pos % self.chunkSize) + 0, (y_pos % self.chunkSize) + 1, (z_pos % self.chunkSize) + 1]
        ]
        
    def loadChunk(self, mapRef):
        self.generateChunk(mapRef)

    def generateChunk(self, mapRef):
        self.chunks[mapRef] = numpy.zeros((self.chunkSize + 1, self.chunkSize + 1, self.chunkSize + 1), float)

        for x in range(self.chunkSize + 1):
            for y in range(self.chunkSize + 1):
                for z in range(self.chunkSize + 1):
                    point = (
                        (mapRef[0] * self.chunkSize) + x,
                        (mapRef[1] * self.chunkSize) + y,
                        (mapRef[2] * self.chunkSize) + z
                        )
                    self.chunks[mapRef][x,y,z] = self.terrainGen.getGeneratedPoint(point)

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

    def __init__(self, chunkSize, seaLevel, seed):
        self.chunkSize = chunkSize
        self.seaLevel = seaLevel
        self.heightMap = PerlinNoise2(16.0,16.0, 256, seed)
        self.noise = PerlinNoise3(4.0,4.0,16.0, 256, seed)

    def getGeneratedPoint(self, point):
        density =  cmp(point[2], self.seaLevel)
        density+=  self.noise(point[0] / 2.0, point[1] / 2.0, point[2] / 2.0) * 2
        density+=  self.noise(point[0] / 2.0, point[1] / 2.0, point[2] / 2.0) * 2

        density+= (float(point[2]) / 2) / self.seaLevel

        #if(density > 0): density = 0
        return density

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

    edges = [
        [0,1],
        [1,2],
        [2,3],
        [3,0],
        [4,5],
        [5,6],
        [6,7],
        [7,4],
        [0,4],
        [1,5],
        [2,6],
        [3,7]
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
    ooo        ooooo                              oooo         o8o                              .oooooo.                .o8                          
    `88.       .888'                              `888         `"'                             d8P'  `Y8b              "888                          
     888b     d'888   .oooo.   oooo d8b  .ooooo.   888 .oo.   oooo  ooo. .oo.    .oooooooo    888          oooo  oooo   888oooo.   .ooooo.   .oooo.o 
     8 Y88. .P  888  `P  )88b  `888""8P d88' `"Y8  888P"Y88b  `888  `888P"Y88b  888' `88b     888          `888  `888   d88' `88b d88' `88b d88(  "8 
     8  `888'   888   .oP"888   888     888        888   888   888   888   888  888   888     888           888   888   888   888 888ooo888 `"Y88b.  
     8    Y     888  d8(  888   888     888   .o8  888   888   888   888   888  `88bod8P'     `88b    ooo   888   888   888   888 888    .o o.  )88b 
    o8o        o888o `Y888""8o d888b    `Y8bod8P' o888o o888o o888o o888o o888o `8oooooo.      `Y8bood8P'   `V88V"V8P'  `Y8bod8P' `Y8bod8P' 8""888P' 
                                                                                d"     YD                                                            
                                                                                "Y88888P'                                                            
    '''                                                                                                                                                  
    @staticmethod
    def marchingCubes(chunkData, size, offset, scale):
        snode = GeomNode('chunk')

        x_count = size[0]
        y_count = size[1]
        z_count = size[2]

        array = GeomVertexArrayFormat()
        array.addColumn(InternalName.make('vertex'), 3, Geom.NTFloat32, Geom.CPoint)
        array.addColumn(InternalName.make('normal'), 3, Geom.NTFloat32, Geom.CPoint)
        array.addColumn(InternalName.make('color'), 4, Geom.NTFloat32, Geom.CPoint)
        array.addColumn(InternalName.make('texcoord'), 4, Geom.NTFloat32, Geom.CPoint)
        
        format = GeomVertexFormat()
        format.addArray(array)
        format = GeomVertexFormat.registerFormat(format)
        
        vdata=GeomVertexData('strip', format, Geom.UHDynamic)
        
        cube = Geom(vdata)

        vertex = GeomVertexWriter(vdata, 'vertex')
        
        normal = GeomVertexWriter(vdata, 'normal')

        color  = GeomVertexWriter(vdata, 'color')

        texcoord = GeomVertexWriter(vdata, 'texcoord')

        tris = GeomTriangles(Geom.UHDynamic)

        pointHash = {}

        triVerts = [0,0,0]

        isolevel = 0

        for z in range(z_count):
            for y in range(y_count):
                for x in range(x_count):
                        value = 0

                        pointVals = chunkData.getCacheGroup(x + offset[0], y + offset[1], z + offset[2])
                        for i in range(8):
                            if(pointVals[i] < isolevel): 
                                value |= (2 ** i)

                        for tri in range(0, len(cubeTris[value])-1, 3):
                            for vert in range(3):
                                if value is not 0 and value is not 255:
                                    edge = cubeTris[value][tri+vert]
                                    #are these points hashed?
                                    p1 = Voxel.points[Voxel.edges[edge][0]]
                                    p2 = Voxel.points[Voxel.edges[edge][1]]
                                    p1val = pointVals[Voxel.edges[edge][0]]
                                    p2val = pointVals[Voxel.edges[edge][1]]


                                    location = (p1[0]+x, p1[1]+y, p1[2] +z, p2[0] + x, p2[1] + y, p2[2] + z)

                                    if(location in pointHash):
                                        triVerts[vert] = pointHash[location]
                                    else:
                                        if(abs(isolevel - p1val) < 0.00001):
                                            point = p1
                                        if(abs(isolevel - p2val) < 0.00001):
                                            point = p2
                                        if(abs(p1val - p2val) < 0.00001):
                                            point = p1
                                        mu = (isolevel - p1val) / (p2val - p1val)

                                        point = [
                                            p1[0] + mu * (p2[0] - p1[0]),
                                            p1[1] + mu * (p2[1] - p1[1]),
                                            p1[2] + mu * (p2[2] - p1[2])
                                        ]

                                        row = vertex.getWriteRow()

                                        vertex.addData3f(x+point[0]+offset[0],y+point[1]+offset[1],z+point[2]+offset[2])
                                        texcoord.addData3f(point[0],point[1],point[2])
                                        normal.addData3f(0.0,0.0,0.0)
                                        color.addData4f(1.0,1.0,1.0,1.0)

                                        pointHash[location] = row
                                        triVerts[vert] = row

                            tris.addVertices(triVerts[0], triVerts[1], triVerts[2])
        
        vertexReader = GeomVertexReader(vdata, 'vertex')
        normalReader = GeomVertexReader(vdata, 'normal')

        for tri in range(tris.getNumPrimitives()):
            vIndex = tris.getPrimitiveStart(tri)

            vertexReader.setRow(tris.getVertex(vIndex))
            vert1 = vertexReader.getData3f()
            vertexReader.setRow(tris.getVertex(vIndex + 1))
            vert2 = vertexReader.getData3f()
            vertexReader.setRow(tris.getVertex(vIndex + 2))
            vert3 = vertexReader.getData3f()

            triNormal = (vert2 - vert1).cross(vert1 - vert3)
            
            for i in range(3):
                normalReader.setRow(tris.getVertex(vIndex + i))
                newNormal = normalReader.getData3f() + triNormal
                newNormal.normalize()
                normal.setRow(tris.getVertex(vIndex + i))
                normal.setData3f(newNormal)
            
        cube.addPrimitive(tris)
        snode.addGeom(cube)

        return snode

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