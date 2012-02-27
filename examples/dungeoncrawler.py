import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import math

game_time= None
player_model = None
other_model = None

WIDTH = 10
HEIGHT = 8
MAP =[[0,0,0,0,0,0,0,0,0,0,0],
      [0,1,1,1,1,1,1,1,0,1,0],
      [0,1,1,1,1,1,1,1,0,1,0],
      [0,1,1,1,1,1,1,1,0,1,0],
      [0,1,1,1,1,1,1,1,0,1,0],
      [0,1,1,1,1,1,1,1,1,1,0],
      [0,1,0,0,1,1,0,0,1,0,0],
      [0,1,0,1,1,1,1,1,1,0,0],
      [0,0,0,0,0,0,0,0,0,0,0]]


class Player (object):

    def __init__(self):
        self.x, self.y, self.z = 2.8,3.1,0.0
        self.zoom = 4.0
        self.rotz = 0.0
        self.accel = 0.001
        self.max_speed = 0.02
        self.speed = 0.0
        self.state = None

    def move_forward(self):
        print "starting forward movement"
        self.state = "moving_forward"

    def move_back(self):
        print "starting back movement"
        self.state = "moving_back"

    def turn_right(self):
        print "turning red"
        self.state = "turning_right"

    def turn_left(self):
        print "turning left"
        self.state = "turning_left"

    def mouse_rotate(self, amt):
        self.rotz += amt / 3

    def stop(self):
        self.state = None
        self.speed = 0.0
        print self.x, self.y, self.z

    def update(self):
        if self.state == "moving_forward":
            self.x -= math.cos(math.radians(self.rotz))* -1.0  * self.speed
            self.y -= math.sin(math.radians(self.rotz)) * self.speed
        elif self.state == "moving_back":
            self.x += math.cos(math.radians(self.rotz))* -1.0  * self.speed
            self.y += math.sin(math.radians(self.rotz)) * self.speed
        elif self.state == "turning_right":
            self.rotz += self.speed +2.0
        elif self.state == "turning_left":
            self.rotz -= self.speed +2.0

        if self.state:
            if self.speed < self.max_speed:
               self.speed += self.accel
               #print self.speed


class Model(object):
    def __init__(self, image):
        self.image = pygame.image.load(image)
        self.display_list = glGenLists(1);
        self.build_display_list()

    def build_display_list(self):
        glNewList(self.display_list, GL_COMPILE)
        SIZE = 0.01
        WIDTH, HEIGHT = self.image.get_size()
        glTranslatef( 0.0, (-1.0 * SIZE) * (WIDTH/2.0),	SIZE * HEIGHT)
        for y in range(0, HEIGHT):
            for x in range(0, WIDTH):
                glTranslatef(0.0, SIZE, 0.0)
                if not self.image.get_at((x,y)) == (255,0,255):
                    self.draw_colored_box(SIZE, self.image.get_at((x,y)) )
            glTranslatef( 0.0, (WIDTH)* (-1.0 * SIZE), (-1.0 * SIZE))
        glEndList()

    def draw_colored_box(self, size, color):
        glColor3f(float(color[0])/255.0,float(color[1])/255.0,float(color[2])/255.0)
        glutSolidCube(size)

def init(width, height):
    pygame.init()
    glutInit([], [])
    screen = pygame.display.set_mode((width, height), OPENGL|DOUBLEBUF|RESIZABLE)
    glEnable(GL_COLOR_MATERIAL);
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
    gluPerspective(40.0, float(width)/float(height), 0.1, 1000.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

    return screen


def load_textures():
    image = pygame.image.load('assets/wall1.bmp')
    ix, iy = image.get_width(), image.get_height()
    image = pygame.image.tostring(image, 'RGBX', True)

    glPixelStorei(GL_UNPACK_ALIGNMENT,1)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, ix, iy, 0, GL_RGBA, GL_UNSIGNED_BYTE, image)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)


def draw_room(x, y):
    #glEnable(GL_CULL_FACE)
    #glEnable(GL_DEPTH_TEST)
    glBegin(GL_QUADS)
    glColor3f(1.0, 1.0, 1.0)
    glNormal3f(0.0, 0.0, 0.5)
    glTexCoord2f(0.5, 0.0); glVertex3f( 0.5, 0.0, 0.0)
    glNormal3f(0.0, 0.0, 1.0)
    glTexCoord2f(0.0, 0.0); glVertex3f( 0.5, 0.5, 0.0)
    glNormal3f(0.0, 0.0, 1.0)
    glTexCoord2f(0.0, 0.5); glVertex3f( 0.0, 0.5, 0.0)
    glNormal3f(0.0, 0.0, 1.0)
    glTexCoord2f(0.5, 0.5); glVertex3f( 0.0, 0.0, 0.0)        ##bottom

    if not MAP[y][x-1]:
        glNormal3f(1.0, 0.0, 0.0)
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.0, 0.0, 0.0)
        glNormal3f(1.0, 0.0, 0.0)
        glTexCoord2f(1.0, 0.5); glVertex3f( 0.0, 0.5, 0.0)
        glNormal3f(1.0, 0.0, 0.0)
        glTexCoord2f(0.5, 0.5); glVertex3f( 0.0, 0.5, 0.5)
        glNormal3f(1.0, 0.0, 0.0)
        glTexCoord2f(0.5, 1.0); glVertex3f( 0.0, 0.0, 0.5)


    if not MAP[y][x+1]:
        glNormal3f(-1.0, 0.0, 0.0)
        glTexCoord2f(0.5, 1.0); glVertex3f( 0.5, 0.0, 0.5)
        glTexCoord2f(0.5, 0.5); glVertex3f( 0.5, 0.5, 0.5)
        glTexCoord2f(1.0, 0.5); glVertex3f( 0.5, 0.5, 0.0)
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.5, 0.0, 0.0)

    if not MAP[y+1][x]:
        glNormal3f(1.0, 0.0, 0.0)
        glTexCoord2f(1.0, 0.5); glVertex3f( 0.5, 0.5, 0.0)
        glTexCoord2f(0.5, 0.5); glVertex3f( 0.5, 0.5, 0.5)
        glTexCoord2f(0.5, 1.0); glVertex3f( 0.0, 0.5, 0.5)
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.0, 0.5, 0.0)

    if not MAP[y-1][x]:
        glNormal3f(1.0, 1.0, 0.0)
        glTexCoord2f(1.0, 1.0); glVertex3f( 0.0, 0.0, 0.0)
        glTexCoord2f(0.5, 1.0); glVertex3f( 0.0, 0.0, 0.5)
        glTexCoord2f(0.5, 0.5); glVertex3f( 0.5, 0.0, 0.5)
        glTexCoord2f(1.0, 0.5); glVertex3f( 0.5, 0.0, 0.0)
    glEnd()


def draw_axis(length=5.0):
    glBegin(GL_LINES)
    glColor3f(0.0, 0.3, 1.0)
    glVertex3f( 0.0, 0.0, 0.0) ## Z Blue
    glVertex3f( 0.0, 0.0, length)

    glColor3f(1.0, 0.0, 0.0) ## Y Red
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(0.0, length, 0.0)

    glColor3f(0.0, 1.0, 0.0)  ## X Green
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(length, 0.0, 0.0)
    glEnd()


def prints(x, y, string):
    length = len(string)
    i = 0
    glRasterPos2f(x, y)
    for c in string:
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(c))


def drawText( value, x,y,  windowHeight, windowWidth, step = 18 ):
    """Draw the given text at given 2D position in window
    """
    glColor3f(1.0, 1.0, 1.0)
    glMatrixMode(GL_PROJECTION);
    # For some reason the GL_PROJECTION_MATRIX is overflowing with a single push!
    # glPushMatrix()
    matrix = glGetDouble( GL_PROJECTION_MATRIX )

    glLoadIdentity();
    glOrtho(0.0, windowHeight or 32, 0.0, windowWidth or 32, -1.0, 1.0)
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    lines = 0
##	import pdb
##	pdb.set_trace()
    for character in value:
        if character == '\n':
            glRasterPos2i(x, y-(lines*18))
        else:
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ord(character));
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    # For some reason the GL_PROJECTION_MATRIX is overflowing with a single push!
    # glPopMatrix();
    glLoadMatrixd( matrix ) # should have un-decorated alias for this...

    glMatrixMode(GL_MODELVIEW);

def resize(width, height):
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45.0, float(width)/float(height), 0.1, 1000.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def draw(player):
    glEnable (GL_LIGHTING)
    px, py, pz, pzoom = player.x, player.y, player.z, player.zoom

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity()

    ly = (math.sin(math.radians(player.rotz))       ) * pzoom
    lx = (math.cos(math.radians(player.rotz)) * -1.0) * pzoom

    x = px + lx
    y = py + ly
    z = pz + pzoom / 2

    gluLookAt(x, y, z, px, py, pz, 0.0 , 0.0 , 1.0)
    draw_axis()

    glPushMatrix()
    glEnable(GL_TEXTURE_2D)
    for y in range(0, HEIGHT):
        for x in range(0, WIDTH):
            glTranslatef(0.5,0.0,0.0)
            if MAP[y][x]:
                draw_room(x,y)
                pass
        glTranslatef((WIDTH)*-0.5,0.5,0.0)
    glDisable(GL_TEXTURE_2D)
    glPopMatrix()



    glPushMatrix()
    glTranslatef(px, py, pz)
    glRotatef(player.rotz, 0.0, 0.0, -1.0)
    draw_axis(0.2)
    glCallList(player_model.display_list)
    glRotatef(player.rotz, 0.0, 0.0, 1.0)
    glTranslatef(0.0, 0.0, 0.8)
    glPopMatrix()

    glutSolidCube(0.1)
    glLightfv (GL_LIGHT0, GL_POSITION, [2.0, 2.0, 0.1, 1.1])
    glLightfv (GL_LIGHT0, GL_AMBIENT, [0.0, 0.0, 0.0, 1.0])
    glLightfv (GL_LIGHT0, GL_DIFFUSE, [0.0, 1.0, 1.0, 1.0])
    glLightfv (GL_LIGHT0, GL_SPECULAR, [0.5, 0.5, 0.5, 1.0])

    glDisable(GL_LIGHTING)
    string = "fps: %d x: %f y: %f z: %f" % (game_time.get_fps(), px, py, pz)
    drawText(string, 10,10, 1000, 900)
    glEnable(GL_LIGHTING)

    glTranslate(0.0, 0.0, 0.0)
    glRotate(90.0, 0.0, 0.0, -1.0)
    draw_axis(0.4)
    glCallList(other_model.display_list)

    pygame.display.flip()


screen = init(1280, 720)
load_textures()
game_time = pygame.time.Clock()
player = Player()
player_model = Model("assets/char.bmp")
other_model = Model("assets/sword.bmp")
run = True
rotating = False
while run:
    player.update()
    draw(player)
    game_time.tick()
    for event in pygame.event.get():
        if event.type == pygame.QUIT: run = False

        elif event.type == MOUSEBUTTONDOWN:
            if event.button == 5:
                player.zoom += 0.1
            if event.button == 4:
                player.zoom -= 0.1
            if event.button == 3:
                rotating = True
            print event.pos, event.button

        elif event.type == MOUSEBUTTONUP:
            if event.button == 3:
                rotating = False

        elif event.type == MOUSEMOTION:
            if rotating:
                player.mouse_rotate(event.rel[0])

        elif event.type == KEYDOWN:
            if event.key == K_ESCAPE:
                run = False
            if event.key == K_w:
                player.move_forward()
            if event.key == K_s:
                player.move_back()
            if event.key == K_a:
                player.turn_left()
            if event.key == K_d:
                player.turn_right()

            if event.key == K_k:
                pzoom += 1.0
            if event.key == K_l:
                pzoom -= 1.0

        elif event.type == KEYUP:
            player.stop()

        elif event.type == VIDEORESIZE:
            print event
            resize(event.w, event.h)

            print game_time.get_fps()