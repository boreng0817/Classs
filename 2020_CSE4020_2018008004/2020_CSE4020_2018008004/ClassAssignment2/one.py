import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
from OpenGL.arrays import vbo
import ctypes

# For dragging
gIsLPressed = False
gIsRPressed = False
gMoveCursor = False
gPosPrev = (0., 0.)
gPosNext = (0., 0.)
gPhi = .1
gTheta = np.pi/2
gScroll = 7
gPosX = 0
gPosY = 0

# ClassAssignment2
gFace = [ [], [], [] ]
gVNorm = [ [], [], [] ]
gVertex = [(0,0,0)]
gNorm = []
gShading = []
gSmooth = []
gDrawWire = False
gSmoothShading = False

def render():
    global gPosNext, gPosPrev, gPhi, gTheta, gPosY
    global gPosX

    # enable depth test (we'll see details later)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    
    if gDrawWire:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    else:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(90, 1, 1,100)
    
    if gIsLPressed:
        gPhi += np.radians(getDeltaX(gPosNext, gPosPrev))
        gTheta += np.radians(getDeltaY(gPosNext, gPosPrev))
        setTheta()

    if gIsRPressed:
        gPosX += getDeltaX(gPosNext, gPosPrev)
        gPosY -= getDeltaY(gPosNext, gPosPrev)

    eye = getEye(gTheta, gPhi, gScroll)
    
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glTranslatef(gPosX/100,gPosY/100,0)
    gluLookAt( eye[0], eye[1], eye[2], 0,0,0, 0,1,0)
    
    gPosPrev = gPosNext
    
    drawFrame()
    
    #Assignment 2
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glEnable(GL_LIGHT1) #
    glEnable(GL_LIGHT2)
    glEnable(GL_LIGHT3)
    #glEnable(GL_RESCALE_NORMAL)
    glEnable(GL_NORMALIZE)

    glPushMatrix()
    
    light = [GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3]
    lightPos = [(2,2,2,1),(2,2,-2,1),(-2,2,2,1),(0,-5,0,1)]
    lightColor = [(1,.1,.1,0),(.1,1,.1,0),(.1,.1,1,0),(.5,.5,.5,0)]

    for i in range(4):
        glLightfv(light[i], GL_POSITION, lightPos[i])
    glPopMatrix()

    for i in range(4):
        ambientLightColor = (.1,.1,.1,1.)
        glLightfv(light[i], GL_DIFFUSE, lightColor[i])
        glLightfv(light[i], GL_SPECULAR, lightColor[i])
        glLightfv(light[i], GL_AMBIENT, ambientLightColor)

    objectColor = (1.,1.,1.,1.)
    specularObjectColor = (1.,1.,1.,1.)
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, objectColor)
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, 10)
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularObjectColor)

    glPushMatrix()

    glColor3ub(0, 0, 255)
    if gSmoothShading:
        drawSmoothObjFile()
    else:
        drawObjFile()
    glPopMatrix()

    glDisable(GL_LIGHTING)

def setTheta():
    global gTheta
    if gTheta > np.pi:
        gTheta = np.pi

    elif gTheta < 0:
        gTheta = 1e-15

def getEye(theta, phi, leng):
    return( -leng * np.sin(theta)*np.cos(phi), 
            -leng * np.cos(theta), 
            -leng * np.sin(theta)*np.sin(phi),
            )


def getDeltaX(p1, p2):
    return p1[0] - p2[0]

def getDeltaY(p1, p2):
    return p1[1] - p2[1]

def drawFrame():
    glBegin(GL_LINES)
    #white line
    glColor3ub(255,255,255)
    for i in range(-20, 21):
        if i == 0:
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
    glVertex3fv(np.array([20.,0,0.]))
    glVertex3fv(np.array([0.,0,0.]))
    glColor3ub(0, 255, 0)
    glVertex3fv(np.array([0.,0,0.]))
    glVertex3fv(np.array([0.,20.,0.]))
    glColor3ub(0, 0, 255)
    glVertex3fv(np.array([0.,0,20.]))
    glVertex3fv(np.array([0.,0.,0]))
    glEnd()


def mouse_callback(window, button, action, mods):
    global gIsRPressed, gIsLPressed, gMoveCursor
    if button == glfw.MOUSE_BUTTON_LEFT and action == glfw.PRESS:
        gIsLPressed = True
        gMoveCursor = True
    elif button == glfw.MOUSE_BUTTON_LEFT and action == glfw.RELEASE:
        gIsLPressed = False
    if button == glfw.MOUSE_BUTTON_RIGHT and action == glfw.PRESS:
        gIsRPressed = True
        gMoveCursor = True
    elif button == glfw.MOUSE_BUTTON_RIGHT and action == glfw.RELEASE:
        gIsRPressed = False

def cursor_callback(window, x, y):
    global gMoveCursor, gPosNext, gPosPrev
    if (gIsLPressed or gIsRPressed)and not gMoveCursor:
        gPosNext = glfw.get_cursor_pos(window)
    elif (gIsLPressed or gIsRPressed) and gMoveCursor:
        gMoveCursor = False
        gPosNext = gPosPrev = glfw.get_cursor_pos(window)

def scroll_callback(window, xoff, yoff):
    global gScroll

    if gScroll - yoff >= 0:
        gScroll -= yoff

#ClassAssignment2
def drop_callback(window, path):   
    parseObjFile(path[0])
    print("File Name:", path[0])
    print("Total number of faces:", (len(gFace[0])+len(gFace[1])+len(gFace[2])))
    print("Number of faces with 3 vertices:", len(gFace[0]))
    print("Number of faces with 4 vertices:", len(gFace[1]))
    print("Number of faces with more than 4 vertices:", len(gFace[2]))
    triangulation()
    createShadingArr()
    createSmoothSahdingArr()

def createSmoothSahdingArr():
    global gSmooth
    gSmooth = []
    smooth = []
    faceNorm = []
    vertexSet = [] 

    for i in range(len(gVertex)):
        vertexSet.append([])

    for face in gFace[0]:
        face = tuple(map(int, face))
        v1 = np.array(gVertex[face[1]]) - np.array(gVertex[face[0]])
        v2 = np.array(gVertex[face[2]]) - np.array(gVertex[face[0]])
        vNorm = np.cross(v1, v2)
        vNorm /= np.linalg.norm(vNorm)
        faceNorm.append(vNorm)
        for i in face:
            vertexSet[i].append(np.array(vNorm))

    smooth.append((0,0,0))
    for i in range(1,len(vertexSet)):
        norm = sum(vertexSet[i])
        norm /= np.linalg.norm(norm)
        smooth.append(norm)

    for i in range(len(gFace[0])):                                              
        for j in range(3):                                                      
            idx1 = int(gFace[0][i][j])                                          
            idx2 = int(gVNorm[0][i][j])                                         
            gSmooth.append(smooth[idx2])                                        
            gSmooth.append(gVertex[idx1])  


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


def createShadingArr():
    global gShading
    gShading = []

    for i in range(len(gFace[0])):
        for j in range(3):
            idx1 = int(gFace[0][i][j])
            idx2 = int(gVNorm[0][i][j])
            gShading.append(gNorm[idx2])
            gShading.append(gVertex[idx1])

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

def drawObjFile():
    global gShading
    varr = np.array(gShading, 'float32')
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_FLOAT, 6*varr.itemsize, varr)
    glVertexPointer(3, GL_FLOAT, 6*varr.itemsize, 
            ctypes.c_void_p(varr.ctypes.data + 3*varr.itemsize))
    glDrawArrays(GL_TRIANGLES, 0, int(varr.size/6))

def drawSmoothObjFile():
    global gSmooth
    varr = np.array(gSmooth, 'float32')
    glEnableClientState(GL_VERTEX_ARRAY)
    glEnableClientState(GL_NORMAL_ARRAY)
    glNormalPointer(GL_FLOAT, 6*varr.itemsize, varr)
    glVertexPointer(3, GL_FLOAT, 6*varr.itemsize, 
            ctypes.c_void_p(varr.ctypes.data + 3*varr.itemsize))
    glDrawArrays(GL_TRIANGLES, 0, int(varr.size/6))


def key_callback(window, key, scancode, action, mods):
    global gDrawWire, gSmoothShading
    if action==glfw.PRESS or action==glfw.REPEAT:
        if key==glfw.KEY_Z:
            gDrawWire = not gDrawWire
        elif key==glfw.KEY_S:
            gSmoothShading = not gSmoothShading

def main():
    if not glfw.init():
        return
    window = glfw.create_window(780,780,'2018008004', None,None)
    if not window:
        glfw.terminate()
        return
    glfw.make_context_current(window)
    glfw.set_scroll_callback(window, scroll_callback)
    glfw.set_mouse_button_callback(window, mouse_callback)
    glfw.set_cursor_pos_callback(window, cursor_callback)
    glfw.set_drop_callback(window, drop_callback)
    glfw.set_key_callback(window, key_callback)

    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
