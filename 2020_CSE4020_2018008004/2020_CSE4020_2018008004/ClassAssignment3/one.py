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

#ClassAssignment3
gOffset = None
gChannel = None
gTree = None
gMotion = None
gName = None
gIndex = None
gFrames = 0
gFrameTime = 0
gTicks = 0
gTickEnable = False
def render():
    global gPosNext, gPosPrev, gPhi, gTheta, gPosY
    global gPosX, gTicks

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
 
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
   
    if gIsLPressed:
        gPhi += np.radians(getDeltaX(gPosNext, gPosPrev))
        gTheta += np.radians(getDeltaY(gPosNext, gPosPrev))
        setTheta()

    if gIsRPressed:
        gPosX += getDeltaX(gPosNext, gPosPrev)
        gPosY -= getDeltaY(gPosNext, gPosPrev)

    eye = getEye(gTheta, gPhi, gScroll)
    
    glTranslatef(gPosX/100,gPosY/100,0)
    gluLookAt( eye[0], eye[1], eye[2], 0,0,0, 0,1,0)
    
    gPosPrev = gPosNext
    
    drawFrame()
    
    if gTickEnable:
        gTicks += 1
    if gTree:
        drawAnimation(int(gTicks/15))


def drawAnimation(tick):
    dfs(gTree, tick)

def dfs(tree, tick):
    if tree[0][-3:] == "End":
        drawEnd(tree[0])
        return

    for i in range(int(len(tree)/2)):
        glPushMatrix()
        drawName(tree[2*i], tick)
        dfs(tree[2*i + 1], tick)
        glPopMatrix()

def drawName(name, tick):
    offset = gOffset[name]
    channel = gChannel[name]
    motion = gMotion[tick % len(gMotion)]
    
    glBegin(GL_LINES)
    if name == "Spine":
        glColor3ub(0, 255, 0)
    elif name in ["RightArm", "LeftArm", "RightUpLeg", "LeftUpLeg"]:
        glColor3ub(0, 0, 255)
    elif name in ["RightForeArm", "LeftForeArm", "RightLeg", "LeftLeg"]:
        glColor3ub(255, 255, 0)
    else:
        glColor3ub(255, 0, 0)
    glVertex3fv(np.array([0, 0, 0,]))
    glVertex3fv(np.array(offset))
    glEnd()

    glTranslatef(offset[0], offset[1], offset[2])

    for i in range(len(channel)):
        val = motion[i + gIndex[name]]
        string = channel[i].upper()
        if string == "XPOSITION":
            glTranslatef(val, 0, 0)
        elif string == "YPOSITION":
            glTranslatef(0, val, 0)
        elif string == "ZPOSITION":
            glTranslatef(0, 0, val)
        elif string == "ZROTATION":
            glRotatef(val, 0, 0, 1)
        elif string == "XROTATION":
            glRotatef(val, 1, 0, 0)
        elif string == "YROTATION":
            glRotatef(val, 0, 1, 0)
    



def drawEnd(name):
    offset = gOffset[name]

    glBegin(GL_LINES)
    glColor3ub(0, 0, 255)
    glVertex3fv(np.array([0, 0, 0,]))
    glVertex3fv(np.array(offset))
    glEnd()

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

#ClassAssignment3
def drop_callback(window, path):
    global gTickEnable, gTicks
    gTickEnable = False
    gTicks = 0
    parseBVHfile(path[0])
    print("File Name:", path[0])
    print("Number of frames:", gFrames)
    print("Frame per second:", int(1/gFrameTime))
    print("Number of joints:", len(gName))
    print("List of all joint names:", gName)

def push(obj, l, depth):
    while depth:
        l = l[-1]
        depth -= 1

    l.append(obj)                                                               

def parseParentheses(arr):
    groups = []                                                                 
    depth = 0                                                                   
                                                                                
    for e in arr:                                                               
        if e == '{':                                                            
            push([], groups, depth)                                             
            depth += 1                                                          
        elif e == '}':                                                          
            depth -= 1                                                          
        else:                                                                   
            push(e, groups, depth)                                              
    return groups

def parseBVHfile(path):
    global gOffset, gChannel, gTree, gMotion
    gMotion = []
    
    f = open(path, "r")
    line = "String"

    while True:
        line = f.readline().split()
        if not line:
            break
        if line[0] == "HIERARCHY":
            gOffset, gChannel, gTree = parseHierarchy(f)
        elif line[0] == "MOTION":
            gMotion = parseMotion(f)
    return

def parseHierarchy(f):
    global gName, gIndex
    gName = []
    gIndex = {}

    offset = {}
    channel = {}
    tree = []
    line = "string"
    depth = 0
    root = True
    name = "string"
    prevname = "string"
    while root or depth != 0:
        line = f.readline().split()

        if line[0] == "ROOT":
            name = line[1]
            tree.append(name)
            gName.append(name)
            gIndex[name] = 0

        elif line[0] == "JOINT":
            prevname = name
            gIndex[line[1]] = gIndex[prevname] + len(channel[prevname])
            name = line[1]
            tree.append(name)
            gName.append(name)
        elif line[0] == "CHANNELS":
            channel[name] = line[2:]
        elif line[0] == "OFFSET":
            offset[name] = list(map(float, line[1:]))
            if name[-3:] == "End":
                name = name[:-3]
        elif line[0] == "End":
            name = name + "End"
            tree.append(name)
        elif line[0] == "{":
            if root:
                root = False
            tree.append("{")
            depth += 1
        elif line[0] == "}":
            tree.append("}")
            depth -= 1
    return offset, channel, parseParentheses(tree)

def parseMotion(f):
    global gFrames, gFrameTime

    motion = []

    line = "string"

    while True:
        line = f.readline().split()

        if not line:
            break
        if line[0] == "Frames:":
            gFrames = int(line[1])
        elif line[0] == "Frame":
            gFrameTime = float(line[2])
        else:
            motion.append(list(map(float, line)))

    motion = [[0.0] * len(motion[0])] + motion
    return motion


def key_callback(window, key, scancode, action, mods):
    global gTickEnable, gMotion
    if action==glfw.PRESS or action==glfw.REPEAT:
        if key==glfw.KEY_SPACE and (not gTickEnable):
            gTickEnable = True
            gMotion = gMotion[1:]

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

    glfw.swap_interval(1)
    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
