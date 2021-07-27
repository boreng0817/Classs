import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
from OpenGL.arrays import vbo
import ctypes
import random

class entity:
    shading = []
    mat = []
    pos = [0, 0, 0]
    movqueue = []
    rotqueue = []
    direct = 1
    scale = 1
    run = 0
    run_count = 0
    run2 = 0
    run2_count = 0

    def __init__(self, shading, pos, q, scale):
        self.shading = shading
        self.pos = pos
        self.movqueue = q
        self.scale = scale
        self.mat = np.identity(4)
        self.enqueueOffset(self.pos)
        self.enqueueRot(1)
        self.applyMove()

    def enqueueOffset(self, offset):
        self.movqueue.append(offset)
    def isQueueEmpty(self):
        return not self.movqueue

    def enqueueRot(self, direct):
        self.rotqueue.append(direct)
    
    def applyRot(self):

        if self.rotqueue[0] == -1:
            self.rotqueue = self.rotqueue[1:]
            return

        self.mat = self.mat @ rotatemat((self.rotqueue[0] - self.direct) * 90)
        self.direct = self.rotqueue[0]
        self.rotqueue = self.rotqueue[1:]
        
    def applyMove(self): 
        if self.run:
            self.run_count -= 1
            if self.run_count == 0:
                self.run = 0
        elif self.run2:
            self.run_count2 -= 1
            if self.run_count2 == 0:
                self.run2 = 0

        offset = self.movqueue[0]

        if self.direct == 0:
            self.pos[0] -= offset[2]
        elif self.direct == 1:
            self.pos[2] += offset[2]
        elif self.direct == 2:
            self.pos[0] += offset[2]
        elif self.direct == 3:
            self.pos[2] -= offset[2]

        self.pos[1] += offset[1]
        self.applyRot()
        self.mat = self.mat @ translateMat(offset)
        self.movqueue = self.movqueue[1:]
    def scalef(self, size):
        self.scale += size
    def shear(self, size):
        self.mat = self.mat @ shearX(size)

    def produceRandomMove(self):
        random_jump = random.randint(0,10)
        random_rotate = random.randint(0,3)
        random_offset = [0, 0, random.randint(0,5)/100]

        if self.run or self.run2:
            return
        if random_jump != 4:
            self.enqueueOffset([0,0,0])
            self.enqueueRot(random_rotate)
            for i in range(20):
                self.enqueueOffset(random_offset)
                self.enqueueRot(random_rotate)
            for i in range(30):
                #padding
                self.enqueueOffset([0,0,0])
                self.enqueueRot(-1)
        else:
            for i in range(20):
                self.enqueueOffset([0, .1, 0])
                self.enqueueRot(-1)
            for i in range(20):
                self.enqueueOffset([0, -.1, 0])
                self.enqueueRot(-1)

    def setRun(self, direct):
        
        self.rotqueue = [direct] + ([-1] * 15) + self.rotqueue
        self.movqueue = [[0,0,0]] + ([[0,0,.2]]*15) + self.movqueue
        self.run = 1
        self.run_count = 15

    def setRun2(self, direct):
        self.rotqueue = [direct] + ([-1] * 15) + self.rotqueue
        self.movqueue = [[0,0,0]] + ([[0,0,.2]]*15) + self.movqueue
        self.run2 = 1
        self.run_count2 = 15

def translateMat(offset):
    mat = np.identity(4)
    mat[0][3] = offset[0]
    mat[1][3] = offset[1]
    mat[2][3] = offset[2]

    return mat
def rotatemat(deg):
    cos = np.cos(np.radians(deg))
    sin = np.sin(np.radians(deg))

    mat = np.identity(4)
    mat[0:3, 0:3] = [[ cos, 0, sin],
                     [ 0,   1,   0],
                     [-sin, 0, cos]]

    return mat
def shearX(size):
    mat = np.identity(4)
    mat[1][0] = size
    mat[2][0] = size

    return mat
# objs
gLego = None
gRiz = None
gBreon = None
gGastly = None
gSun = None
# ClassAssignment2
gFace = [ [], [], [] ]
gVNorm = [ [], [], [] ]
gVertex = [(0,0,0)]
gNorm = []
gShading = []
# Camera
gCameraMode = True
# curve
p0 = np.array([0, 0])
p1 = np.array([0, 0])
p2 = np.array([0, 0])
p3 = np.array([0, 0])



def render():

    # enable depth test (we'll see details later)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(90, 1, 1,100)
    
       
    glMatrixMode(GL_MODELVIEW)

    # CAMERA MODE
    glLoadIdentity()
    if gCameraMode:
        off = gLego.pos
        gluLookAt( 5+off[0],5+off[1],5+off[2] , 
                   off[0]  ,off[1]  ,off[2]   ,
                   0,1,0)
    else:    
        eye = gLego.pos[:]
        eye[1] += 1.5*10/gLego.scale
        hands = .65
        at = []
        if gLego.direct == 1:
            at = [0,0,1]
            eye[2] -= hands
        elif gLego.direct == 2:
            at = [1,0,0]
            eye[0] -= hands
        elif gLego.direct == 3:
            at = [0,0,-1]
            eye[2] += hands
        elif gLego.direct == 0:
            eye[0] += hands
            at = [-1,0,0]

        gluLookAt( eye[0],eye[1],eye[2], 
                eye[0]+at[0], eye[1]+at[1], eye[2]+at[2], 0,1,0)
    
    drawFrame()
    
    #light
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0) #animated light
    glEnable(GL_LIGHT1)
    glEnable(GL_NORMALIZE)

    glPushMatrix()

    t = glfw.get_time()
    lightpos0 = (3*np.cos(t), 3, 3*np.sin(t))
    lightpos1 = (0, 0, 0)
    lightcolor0 = (1,1,1,1)
    lightcolor1 = (0,1,0,0)

    glPushMatrix()
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos0)
    glLightfv(GL_LIGHT1, GL_POSITION, lightpos1)
    glPopMatrix()

    ambientLightColor = (.1,.1,.1,0)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor0)                          
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightcolor0)                         
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLightColor)

    ambientLightColor = (.1,.1,.1,1)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcolor1)                          
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightcolor1)                         
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLightColor)
    
    #pokemons run away when lego comes close (dist 3)
    checkDist()
    checkDistOther()
    #add random movement
    if int(5*t) % 5 == 0:
        for ent in [gRiz, gBreon, gGastly]:
            ent.produceRandomMove()

    ###sun in red
    glPushMatrix()
    objectColor = (1,.3,.3,1.)
    specularObjectColor = (1.,1.,1.,1.)                                         
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 10)                           
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)
    glTranslatef(3*np.cos(t),  3, 3*np.sin(t))
    glScalef(1/5,1/5,1/5)
    drawObjFile(gSun.shading)
    glPopMatrix()
    ###

    #main obj
    glPushMatrix()
    objectColor = (1,0,0,1.)                                             
    specularObjectColor = (1.,1.,1.,1.)                                     
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 10)                           
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)
    drawEntity(gLego)
    glPopMatrix()

    glPushMatrix()
    objectColor = (1.,1,0,1.)                                             
    specularObjectColor = (1.,1.,1.,1.)                                     
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 5)                           
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)
    drawEntity(gRiz)
    glPopMatrix()

    glPushMatrix()
    objectColor = (.5,1,1.,1.)                                             
    specularObjectColor = (1.,1.,1.,1.)                                     
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 5)                           
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)
    drawEntity(gBreon)
    glPopMatrix()

    glPushMatrix()
    objectColor = (.3,.3,.8,1.)                                             
    specularObjectColor = (1.,1.,1.,1.)                                     
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)        
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 10)                           
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)
    drawEntity(gGastly)
    glPopMatrix()

    glPopMatrix()
    glDisable(GL_LIGHTING)

def drawBound(t):
    bound = 1.5
    radious = .3
    for i in range(8):
        glPushMatrix()
        glTranslatef((bound - radious)*np.cos(t+(np.pi*i/4)), 
                0, (bound - radious)*np.sin(t+(np.pi*i/4)))
        glScalef(radious,.00003,radious)
        drawObjFile(gShading)
        glPopMatrix()
    
def checkDist():
    #lego-other objs
    disLimit = 3
   
    for ent in [gRiz, gBreon, gGastly]:
        if not ent.run and dist(gLego.pos, ent.pos) < disLimit:
            #run direction is lego's direct
            ent.setRun(gLego.direct)
def checkDistOther():
    disLimit = 3
    for ent in [gRiz, gBreon, gGastly]:
        for ent1 in [gRiz, gBreon, gGastly]:
            if dist(ent.pos, ent1.pos) < .1:
                continue
            if (not ent.run2) and (not ent1.run2) and\
                    dist(ent.pos, ent1.pos) < disLimit:
                if ent.direct != ent1.direct:
                    ent.setRun2((ent.direct + 1)%4)
                    ent1.setRun2((ent1.direct + 1)%4)
                else:
                    ent.setRun2((ent.direct+2)%4)
                    ent1.setRun2((ent1.direct)%4)
def dist(a, b):
    return np.linalg.norm(np.array(a) - np.array(b)) 
def drawFrame():                                                                
    glBegin(GL_LINES)                                                           
    #white line                                                                 
    glColor3ub(100,100,100)                                                     
    for i in range(-20, 21):                                                    
        if i == 0:                                                              
            glVertex3fv(np.array([.3,0,0]))                                     
            glVertex3fv(np.array([20,0,0]))                                     
            glVertex3fv(np.array([0,0,.3]))                                     
            glVertex3fv(np.array([0,0,20]))                                     
            glVertex3fv(np.array([0,0,i]))                                      
            glVertex3fv(np.array([-20.,0,i]))                                   
            glVertex3fv(np.array([i,0.,0]))                                     
            glVertex3fv(np.array([i,0.,-20]))                                   
        else:                                                                   
            glVertex3fv(np.array([20,0,i]))                                     
            glVertex3fv(np.array([-20.,0,i]))                                   
            glVertex3fv(np.array([i,0.,20]))                                    
            glVertex3fv(np.array([i,0.,-20]))                                   
                                                                                
    glColor3ub(255, 0, 0)                                                       
    glVertex3fv(np.array([.3,0,0.]))                                            
    glVertex3fv(np.array([0.,0,0.]))                                            
    glColor3ub(0, 255, 0)                                                       
    glVertex3fv(np.array([0.,0,0.]))                                            
    glVertex3fv(np.array([0.,.3,0.]))                                           
    glColor3ub(0, 0, 255)                                                       
    glVertex3fv(np.array([0.,0,.3]))                                            
    glVertex3fv(np.array([0.,0.,0]))                                            
    glEnd()





    # A-1
def drawEntity(ent):
    glPushMatrix()
    if not ent.isQueueEmpty():
        ent.applyMove()
    glMultMatrixf(ent.mat.T)
    glScalef(1/ent.scale, 1/ent.scale, 1/ent.scale)
    drawObjFile(ent.shading)
    glPushMatrix()
    glScalef(ent.scale, ent.scale, ent.scale)
    drawBound(glfw.get_time())
    glPopMatrix()
    glPopMatrix()

def initObject():
    global gLego, gRiz, gBreon, gGastly, gSun
    path = ["objLego.obj", "objriz.obj", "objumbreon.obj", "objGastly.obj"]

    gLego = entity(makeShading(path[0]), [0,0,0], [], 10)
    gRiz = entity(makeShading(path[1]), [3,0,3], [], 10)
    gBreon = entity(makeShading(path[2]), [5,0.3,-5], [], 3)
    gGastly = entity(makeShading(path[3]), [-2,0,2], [], 5)
    gSun = entity(makeShading("sphere-tri.obj"), [0,0,0], [], 1)
    ##sun
    makeShading("cylinder-tri.obj")

def makeShading(path):   
    parseObjFile(path)
    triangulation()
    createShadingArr()
    return gShading

#simple triangulation algorithm
def triangulation():
    global gFace, gVNorm

    for i in range(len(gFace[1])):
        face = gFace[1][i]
        norm = gVNorm[1][i]

        q,w,e,r = face
        a,s,d,f = norm

        gFace[0].append((q,w,e))
        gFace[0].append((q,e,r))
        gVNorm[0].append((a,s,d))
        gVNorm[0].append((a,d,f))

    for i in range(len(gFace[2])):
        face = gFace[2][i]
        norm = gVNorm[2][i]

        for j in range(len(face) - 2):
            gFace[0].append((face[0], face[j+1], face[j+2]))
            gVNorm[0].append((norm[0], norm[j+1], norm[j+2]))

def parseObjFile(path):
    global gFace, gNorm, gVertex, gVNorm
    
    gFace = [[], [], []]
    gVNorm = [[],[],[]]
    gNorm = [(0,0,0)]
    gVertex = [(0,0,0)]
    f = open(path, "r")
    line = "string"

    while True:
        line = f.readline()
        if not line:
            break
        a = line.split()
        if a == []:
            continue
        elif a[0] == "f":
            b = map(lambda x: x.split('/'), a[1:])
            face = []
            faceNorm = []
            for i in range(len(a) - 1):
                temp = next(b)
                face.append(temp[0])
                faceNorm.append(temp[2])
            if len(a) < 6:
                gFace[len(a) - 4].append(tuple(face))
                gVNorm[len(a) - 4].append(tuple(faceNorm))
            else:
                gFace[2].append(tuple(face))
                gVNorm[2].append(tuple(faceNorm))
        elif a[0] == "v":
            q, w, e = map(float, a[1:])
            gVertex.append((q,w,e))
        elif a[0] == "vn":
            q, w, e = map(float, a[1:])
            gNorm.append((q,w,e))

def createShadingArr():                                                         
    global gShading                                                             
    gShading = []                                                               
                                                                                
    for i in range(len(gFace[0])):                                              
        for j in range(3):                                                      
            idx1 = int(gFace[0][i][j])                                          
            idx2 = int(gVNorm[0][i][j])                                         
            gShading.append(gNorm[idx2])                                        
            gShading.append(gVertex[idx1])

def drawObjFile(shading):
    varr = np.array(shading, 'float32')
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_FLOAT, 6*varr.itemsize, varr)
    glVertexPointer(3, GL_FLOAT, 6*varr.itemsize, 
            ctypes.c_void_p(varr.ctypes.data + 3*varr.itemsize))
    glDrawArrays(GL_TRIANGLES, 0, int(varr.size/6))


def key_callback(window, key, scancode, action, mods):
    global gCameraMode
    if action==glfw.PRESS:
        if key==glfw.KEY_UP:
            for i in range(10):
                gLego.enqueueOffset([0,0,.05])
                gLego.enqueueRot(-1)
        elif key==glfw.KEY_DOWN:
            gLego.enqueueOffset([0,0,0])
            gLego.enqueueRot(abs((2+gLego.direct)%4))
        elif key==glfw.KEY_LEFT:
            gLego.enqueueOffset([0,0,0])
            gLego.enqueueRot(abs((gLego.direct + 1) % 4))
        elif key==glfw.KEY_RIGHT:
            gLego.enqueueOffset([0,0,0])
            gLego.enqueueRot(abs((gLego.direct - 1) % 4))
        elif key==glfw.KEY_SPACE:
            for i in range(10):
                gLego.enqueueOffset([0,.2,0])
                gLego.enqueueRot(-1)
            for i in range(10):
                gLego.enqueueOffset([0,-.2,0])
                gLego.enqueueRot(-1)
        ## scale
        elif key==glfw.KEY_U:
            gLego.scalef(.1)
        elif key==glfw.KEY_I:
            gLego.scalef(-.1)
        elif key==glfw.KEY_J:
            gLego.shear(.1)
        elif key==glfw.KEY_K:
            gLego.shear(-.1)
        elif key==glfw.KEY_T:
            gCameraMode = not gCameraMode

def lerp(t, p1, p2):
    return (1-t)*p1 + t*p2

def deCastelJau(t, pts):
    if len(pts) == 2:
        return lerp(t, pts[0], pts[1])
    else:
        return deCastelJau(t, aux(t, pts))

def aux(t, pts):
    q = []
    for i in range(len(pts) - 1):
        q.append(lerp(t, pts[i], pts[i+1]))
    return q

def main():
    if not glfw.init():
        return
    window = glfw.create_window(780,780,'2018008004', None,None)
    if not window:
        glfw.terminate()
        return
    glfw.make_context_current(window)
    glfw.set_key_callback(window, key_callback)

    initObject()
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
