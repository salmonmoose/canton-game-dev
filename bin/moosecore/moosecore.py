import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math

import os.path

'''
 d888b   .d8b.  .88b  d88. d88888b 
88' Y8b d8' `8b 88'YbdP`88 88'     
88      88ooo88 88  88  88 88ooooo 
88  ooo 88~~~88 88  88  88 88~~~~~ 
88. ~8~ 88   88 88  88  88 88.     
 Y888P  YP   YP YP  YP  YP Y88888P
'''
class Game:
        def __init__(self):
                self.screen = self.init_screen(1280,720)
                pygame.mouse.set_visible(False)
                pygame.event.set_grab(True)
                self.running = True
                self.debug = True
                self.world = World(self)
                self.clock = pygame.time.Clock()
                self.deltaTime = 0
                self.ticks = 0
                self.camera = Camera()
                self.watermark = Text(20,20, 'PyMine')
                self.deltaTime_counter = Text(20,40, self.deltaTime)
                self.world.add_child(Console(self))
                self.world.add_child(self.watermark)
                self.world.add_child(self.deltaTime_counter)
                self.world.add_child(self.camera)
                
        def start(self):
            self.running = True
            while self.running:
                self.clock.tick()
                self.deltaTime = self.clock.get_time()

                self.deltaTime_counter.value = str(self.deltaTime)

                #print(self.ticks)
                self.world.events()
                self.world.update()
                self.world.draw()

        def init_screen(self, width, height):
                pygame.init()
                glutInit([],[])
                screen = pygame.display.set_mode((width, height), OPENGL|DOUBLEBUF|RESIZABLE)
                glEnable(GL_COLOR_MATERIAL)
                glEnable(GL_LIGHTING)
                glEnable(GL_LIGHT0)
                glClearColor(0.0, 0.5, 1.0, 0.0)
                glClearDepth(1.0)
                glDepthFunc(GL_LESS)
                glEnable(GL_DEPTH_TEST)
                glShadeModel(GL_SMOOTH)
                glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
                glMatrixMode(GL_PROJECTION)
                glLoadIdentity()
                gluPerspective(40.0, float(width)/float(height), 0.1, 50000.0)
                glMatrixMode(GL_MODELVIEW)
                glLoadIdentity()
        
                return screen



'''
 d888b   .d8b.  .88b  d88. d88888b  .d88b.  d8888b.    d88b d88888b  .o88b. d888888b 
88' Y8b d8' `8b 88'YbdP`88 88'     .8P  Y8. 88  `8D    `8P' 88'     d8P  Y8 `~~88~~' 
88      88ooo88 88  88  88 88ooooo 88    88 88oooY'     88  88ooooo 8P         88    
88  ooo 88~~~88 88  88  88 88~~~~~ 88    88 88~~~b.     88  88~~~~~ 8b         88    
88. ~8~ 88   88 88  88  88 88.     `8b  d8' 88   8D db. 88  88.     Y8b  d8    88    
 Y888P  YP   YP YP  YP  YP Y88888P  `Y88P'  Y8888P' Y8888P  Y88888P  `Y88P'    YP   
'''
class GameObject:
        def __init__(self, game = None):
                self.children = []
                self.parent = None
                self.game = game
                self.offset     = ( 0, 0, 0)
                self.up         = ( 0, 1, 0)
                self.direction  = ( 1, 0, 0)
                self.scale      = ( 1, 1, 1)
                
        def draw_axis(length=5.0):
                glDisable(GL_LIGHTING)
                glBegin(GL_LINES)
                glColor3f ( 1.0, 0.0, 0.0)
                glVertex3f( 0.0, 0.0, 0.0)
                glVertex3f( 1.0, 0.0, 0.0)
                glColor3f ( 0.0, 1.0, 0.0)
                glVertex3f( 0.0, 0.0, 0.0)
                glVertex3f( 0.0, 1.0, 0.0)
                glColor3f ( 0.0, 0.0, 1.0)
                glVertex3f( 0.0, 0.0, 0.0)
                glVertex3f( 0.0, 0.0, 1.0)
                glEnd()
                glEnable(GL_LIGHTING)

        def draw_voxel(self, center_x, center_y, center_z, size, faces):
                """Draws a voxel"""
                for face in faces:
                        self.draw_face(face, center_x, center_y, center_z, size)
              
        def draw_face(self, face, center_x, center_y, center_z, size):
                """Draws a face for a voxel"""
                glColor4f(1.0, 1.0, 1.0, 0.02)

                for vertex in range(4):
                        glNormal3f(
                                normals[face][0],
                                normals[face][1],
                                normals[face][2]
                        )

                        glTexCoord2f(
                                texCoords[vertex][0],
                                texCoords[vertex][1]
                        )

                        glVertex3f(
                                center_x + voxel[face][vertex][0] * (size / 2),
                                center_y + voxel[face][vertex][1] * (size / 2),
                                center_z + voxel[face][vertex][2] * (size / 2)
                        )

        def add_child(self, child):
                self.children.append(child)
                child.parent = self
                child.set_game(self.game)

        def set_game(self, game):
                self.game = game

        def x_pos(self):
                if(self.parent):
                        return self.parent.x_pos() + self.offset[0]
                else:
                        return self.offset[0]

        def y_pos(self):
                if(self.parent):
                        return self.parent.y_pos() + self.offset[1]
                else:
                        return self.offset[1]

        def z_pos(self):
                if(self.parent):
                        return self.parent.z_pos() + self.offset[2]
                else:
                        return self.offset[2]

        def goPosition3f(x, y, z):
                self.offset = (x,y,z)
                
        def events(self, event=None):
                if event:
                        for child in self.children:
                                child.events(event)
                else:
                        for event in pygame.event.get():
                                self.events(event)
                                for child in self.children:
                                        child.events(event)

        def update(self):
                for child in self.children:
                        child.update()

        def load_textures(self):
                image = pygame.image.load(os.path.join('resources', 'texturepack.png'))
                image_x, image_y = image.get_width(), image.get_height()
                image = pygame.image.tostring(image, 'RGBX', True)

                glPixelStorei(GL_UNPACK_ALIGNMENT,1)
                glTexImage2D(GL_TEXTURE_2D, 0, 3, image_x, image_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)

        def draw(self):
                glPushMatrix()
                glTranslate(self.offset[0], self.offset[1], self.offset[2])
                if self.game.debug:
                        self.draw_axis()
                for child in self.children:
                        child.draw()
                glPopMatrix()

'''
 .o88b.  .d8b.  .88b  d88. d88888b d8888b.  .d8b.  
d8P  Y8 d8' `8b 88'YbdP`88 88'     88  `8D d8' `8b 
8P      88ooo88 88  88  88 88ooooo 88oobY' 88ooo88 
8b      88~~~88 88  88  88 88~~~~~ 88`8b   88~~~88 
Y8b  d8 88   88 88  88  88 88.     88 `88. 88   88 
 `Y88P' YP   YP YP  YP  YP Y88888P 88   YD YP   YP
'''
class Camera(GameObject):
        def __init__(self, game=None):
                GameObject.__init__(self, game)
                self.offset = [ 0, 2000, 0]
                self.pitch = 0.0
                self.yaw = 0.0
                self.speed = 3
                self.x_sensitivity = 0.1
                self.keymap = {'forward': K_w, 'backward': K_s, 'left': K_a, 'right': K_d }
                self.motion = {'forward': False, 'backward': False, 'left': False, 'right': False}

        def draw(self):
                glRotatef(self.yaw,   1.0, 0.0, 0.0)
                glRotatef(self.pitch, 0.0, 1.0, 0.0)

                glTranslated(-self.offset[0], -self.offset[1], -self.offset[2])

                GameObject.draw(self)

        def update(self):
                GameObject.update(self)

                pitchRads = math.radians(self.pitch)
                yawRads = math.radians(self.yaw)

                if self.motion['forward']:
                        self.offset[0] += math.sin(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[2] -= math.cos(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[1] -= math.sin(yawRads)   * self.speed * self.game.deltaTime
                if self.motion['backward']:
                        self.offset[0] -= math.sin(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[2] += math.cos(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[1] += math.sin(yawRads)   * self.speed * self.game.deltaTime
                if self.motion["left"]:
                        self.offset[0] -= math.cos(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[2] -= math.sin(pitchRads) * self.speed * self.game.deltaTime
                if self.motion["right"]:
                        self.offset[0] += math.cos(pitchRads) * self.speed * self.game.deltaTime
                        self.offset[2] += math.sin(pitchRads) * self.speed * self.game.deltaTime
                
        def events(self, event=None):
                GameObject.events(self, event)

                if event:
                        if event.type == MOUSEMOTION:
                                self.pitch += event.rel[0] * self.x_sensitivity
                                self.yaw += event.rel[1] * self.x_sensitivity

                        #FIXME This should be done at a higher level (probably at the Game level) and passed down to GameObjects
                        if event.type == KEYDOWN or event.type == KEYUP:
                                for key in self.keymap.keys():
                                        if event.key == self.keymap[key]:
                                                if event.type == KEYDOWN:
                                                        self.motion[key] = True
                                                else:
                                                        self.motion[key] = False
                        
'''
db      d888888b  d888b  db   db d888888b 
88        `88'   88' Y8b 88   88 `~~88~~' 
88         88    88      88ooo88    88    
88         88    88  ooo 88~~~88    88    
88booo.   .88.   88. ~8~ 88   88    88    
Y88888P Y888888P  Y888P  YP   YP    YP    
'''
class Light(GameObject):
        def __init__(self, game=None):
                GameObject.__init__(self, game)
                self.offset = ( 10,-10, 10)

        def draw(self):
                GameObject.draw(self)
                glLightfv (GL_LIGHT0, GL_POSITION, [2.0, self.offset[0], self.offset[1], self.offset[2]])
                glLightfv (GL_LIGHT0, GL_AMBIENT,  [0.0, 0.0, 0.0, 1.0])
                glLightfv (GL_LIGHT0, GL_DIFFUSE,  [0.0, 1.0, 1.0, 1.0])
                glLightfv (GL_LIGHT0, GL_SPECULAR, [0.5, 0.5, 0.5, 1.0])


'''
db   d8b   db  .d88b.  d8888b. db      d8888b. 
88   I8I   88 .8P  Y8. 88  `8D 88      88  `8D 
88   I8I   88 88    88 88oobY' 88      88   88 
Y8   I8I   88 88    88 88`8b   88      88   88 
`8b d8'8b d8' `8b  d8' 88 `88. 88booo. 88  .8D 
 `8b8' `8d8'   `Y88P'  88   YD Y88888P Y8888D' 
'''
class World(GameObject):
        def draw(self):
                glEnable(GL_LIGHTING)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
                glLoadIdentity()
                
                glPushMatrix()
                GameObject.draw(self)
                glPopMatrix()

                pygame.display.flip()
        
        def events(self, event=None):
                GameObject.events(self, event)
                
                if event:
                        if event.type == pygame.QUIT:
                                self.game.running = False
                        if event.type == pygame.KEYDOWN:
                                if event.key == K_ESCAPE:
                                        self.game.running = False

class UserInterface(GameObject):
        def __init__(self, game=None):
                GameObject.__init__(self, game)

        def draw(self):
                GameObject.draw(self)

class Text(GameObject):
        def __init__(self, x_pos, y_pos, value, game=None):
                GameObject.__init__(self, game)
                self.x_pos = x_pos
                self.y_pos = y_pos
                self.value = value

        def draw(self):

                glMatrixMode(GL_PROJECTION)

                matrix = glGetDouble(GL_PROJECTION_MATRIX)

                glLoadIdentity()
                glOrtho(0.0, self.game.screen.get_width(), 0.0, self.game.screen.get_height(), -1.0, 1.0)
                glMatrixMode(GL_MODELVIEW)
                glPushMatrix()
                glLoadIdentity()
                glDisable(GL_LIGHTING)
                glColor3f(1.0, 1.0, 1.0)
                glRasterPos2i(self.x_pos, self.y_pos)
                for character in self.value:
                        if character == '\n':
                                glRasterPos2i(self.x_pos, self.y_pos - (lines * 18))
                        else:
                                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character))
                glEnable(GL_LIGHTING)
                glPopMatrix()
                glMatrixMode(GL_PROJECTION)
                glLoadMatrixd(matrix)
                glMatrixMode(GL_MODELVIEW)

class Console(Text):
        def __init__(self, game=None):
                Text.__init__(self, 20, 0, '>', game)
                self.y_pos = self.game.screen.get_height() - 20




'''
db    db  .d88b.  db    db d88888b db      
88    88 .8P  Y8. `8b  d8' 88'     88      
Y8    8P 88    88  `8bd8'  88ooooo 88      
`8b  d8' 88    88  .dPYb.  88~~~~~ 88      
 `8bd8'  `8b  d8' .8P  Y8. 88.     88booo. 
   YP     `Y88P'  YP    YP Y88888P Y88888P 
'''
class Voxel(GameObject):
        voxel = [
                [[ 1, 1,-1],[-1, 1,-1],[-1, 1, 1],[ 1, 1, 1]],
                [[ 1,-1, 1],[-1,-1, 1],[-1,-1,-1],[ 1,-1,-1]],
                [[-1,-1, 1],[ 1,-1, 1],[ 1, 1, 1],[-1, 1, 1]],
                [[ 1,-1,-1],[-1,-1,-1],[-1, 1,-1],[ 1, 1,-1]],
                [[-1,-1,-1],[-1,-1, 1],[-1, 1, 1],[-1, 1,-1]],
                [[ 1,-1, 1],[ 1,-1,-1],[ 1, 1,-1],[ 1, 1, 1]]
        ]
        texCoords = [[0,0], [1,0], [1,1], [0,1]]
        normals = [[ 0, 1, 0],[ 0,-1, 0],[ 0, 0, 1],[ 0, 0,-1],[ 1, 0, 0],[-1, 0, 0]]


        @staticmethod
        def draw(center_x, center_y, center_z, size, faces, data):
                """Draws a voxel"""
                for face in faces:
                        Voxel.face(face, center_x, center_y, center_z, size, data)
        
        @staticmethod
        def face(face, center_x, center_y, center_z, size, data):
                """Draws a face for a voxel"""
                glColor4f(1.0, 1.0, 1.0, 0.02)

                block_x = ((data*16) % 16) * 0.0625
                block_y = ((data*16) / 16) * 0.0625

                for vertex in range(4):
                        glNormal3f(
                                Voxel.normals[face][0],
                                Voxel.normals[face][1],
                                Voxel.normals[face][2]
                        )

                        glTexCoord2f(
                                Voxel.texCoords[vertex][0] * 0.0625 + block_x,
                                Voxel.texCoords[vertex][1] * 0.0625 + block_y
                        )

                        glVertex3f(
                                center_x + Voxel.voxel[face][vertex][0] * (size / 2),
                                center_y + Voxel.voxel[face][vertex][1] * (size / 2),
                                center_z + Voxel.voxel[face][vertex][2] * (size / 2)
                        )
        