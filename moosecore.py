import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math

voxel = [
        [[ 1, 1,-1],[-1, 1,-1],[-1, 1, 1],[ 1, 1, 1]],
        [[ 1,-1, 1],[-1,-1, 1],[-1,-1,-1],[ 1,-1,-1]],
        [[ 1, 1, 1],[-1, 1, 1],[-1,-1, 1],[ 1,-1, 1]],
        [[ 1,-1,-1],[-1,-1,-1],[-1, 1,-1],[ 1, 1,-1]],
        [[-1, 1, 1],[-1, 1,-1],[-1,-1,-1],[-1,-1, 1]],
        [[ 1, 1,-1],[ 1, 1, 1],[ 1,-1, 1],[ 1,-1,-1]]
]
texCoords = [[1,0], [0,0], [0,1], [1,1]]
normals = [[ 1, 0, 0], [-1, 0, 0],[ 0, 1, 0],[ 0,-1, 0],[ 0, 0, 1],[ 0, 0,-1]]
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
                screen = self.init_screen(1280,720)
                game_time = pygame.time.Clock()
                self.run = True
                self.debug = True
                self.world = World(self)

                self.camera = Camera()
                
                self.world.add_child(self.camera)
                
        def start(self):
            self.run = True
            while self.run:
                self.world.events()
                self.world.update()
                self.world.draw()
                #self.run = False

        def init_screen(self, width, height):
                pygame.init()
                glutInit([],[])
                screen = pygame.display.set_mode((width, height), OPENGL|DOUBLEBUF|RESIZABLE)
                glEnable(GL_COLOR_MATERIAL)
                glEnable(GL_LIGHTING)
                glEnable(GL_LIGHT0)
                glClearColor(0.0, 0.0, 0.0, 0.0)
                glClearDepth(1.0)
                glDepthFunc(GL_LESS)
                glEnable(GL_DEPTH_TEST)
                glShadeModel(GL_SMOOTH)
                glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
                glMatrixMode(GL_PROJECTION)
                glLoadIdentity()
                gluPerspective(40.0, float(width)/float(height), 0.1, 5000.0)
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
                self.offset = (0, 0, 0)


                
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

        def draw_voxel(self, center_x, center_y, center_z, size):
                """Draws a voxel"""
                for face in range(6):
                        self.draw_face(face, center_x, center_y, center_z, size)
              
        def draw_face(self, face, center_x, center_y, center_z, size):
                """Draws a face for a voxel"""
                glColor3f(1.0, 1.0, 1.0)

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
                image = pygame.image.load('stone.png')
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
                self.offset = (2000,2000,2000)
                self.lookat = ( 0, 0, 0)

        def draw(self):
                gluLookAt(
                        self.x_pos(), self.y_pos(), self.z_pos(),
                        self.look_x_pos(), self.look_y_pos(), self.look_z_pos(),
                        0.0, 1.0, 0.0
                        )

                GameObject.draw(self)

        def look_x_pos(self):
                if(self.parent):
                        return self.parent.x_pos() + self.lookat[0]
                else:
                        return self.lookat[0]

        def look_y_pos(self):
                if(self.parent):
                        return self.parent.y_pos() + self.lookat[1]
                else:
                        return self.lookat[1]

        def look_z_pos(self):
                if(self.parent):
                        return self.parent.z_pos() + self.lookat[2]
                else:
                        return self.lookat[2]

        def events(self, event=None):
                GameObject.events(self, event)

                if event and event.type == MOUSEMOTION:
                        self.offset = (
                                math.cos(event.pos[0]/100.0) * 2000,
                                self.offset[1],
                                math.sin(event.pos[0]/100.0) * 2000
                                )
                        
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
                
                GameObject.draw(self)

                pygame.display.flip()
        
        def events(self, event=None):
                GameObject.events(self, event)
                
                if event and event.type == pygame.QUIT:
                        self.game.run = False
