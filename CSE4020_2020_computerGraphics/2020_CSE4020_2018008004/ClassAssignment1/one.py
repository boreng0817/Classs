import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

# For dragging
gIsLPressed = False
gIsRPressed = False
gMoveCursor = False
gPosPrev = (0., 0.)
gPosNext = (0., 0.)
gPhi = .1
gTheta = np.pi/2
gScroll = 10
gPosX = 0
gPosY = 0

def render():
    global gPosNext, gPosPrev, gPhi, gTheta, gIsRPressed, gIsLPressed, gPosY
    global gPosX

    # call this at the beginning ofyour render function
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE )  


    # enable depth test (we'll see details later)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)

    glLoadIdentity()
    gluPerspective(60, 1, 1,100)

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
        
    # B.
    t = glfw.get_time()
    glColor3ub(255,0,0)
    drawSphere(2)

    #Mercury
    glPushMatrix()
    glRotatef(t*(270/np.pi), 0, 2, 0)
    glTranslatef(0, 0, 3.5)
    glRotatef(t*(180/np.pi), 1, 0, 0)
    glColor3ub(128,128,128)
    drawSphere(0.5)
    glPopMatrix()

    #Venus
    glPushMatrix()
    glRotatef(t*(180/np.pi), 0, 2, 0)
    glTranslatef(0, 0, 5)
    glRotatef(t*(180/np.pi), 1, 0, 0)
    glColor3ub(255,224,0)
    drawSphere(0.75)
    glPopMatrix()
    
    #Earth
    glPushMatrix()
    glRotatef(t*(90/np.pi), 0, 2, 0)
    glTranslatef(0, 0, 10)
    #Moon
    glPushMatrix()
    glRotatef(t*(180/np.pi), 0, 1, 0)
    glTranslatef(0,0, 2.5)
    #Satellite
    glPushMatrix()
    glRotatef(t*(180/np.pi), 1, 1, 0)
    glTranslatef(0, 0, 0.8)
    glColor3ub(255,255,255) #white
    drawCube()
    glPopMatrix()
    #satellite_end
    glColor3ub(211,211,211) #silver
    drawSphere(0.5)
    glPopMatrix()
    #Moon_end 
    glRotatef(t*(180/np.pi), 1, 0, 0)
    glColor3ub(0,255,0)
    drawSphere(1)
    glPopMatrix()



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
    glColor3ub(255, 0, 0)
    glVertex3fv(np.array([20.,0,0.]))
    glVertex3fv(np.array([-20.,0,0.]))
    glColor3ub(0, 255, 0)
    glVertex3fv(np.array([0.,0,0.]))
    glVertex3fv(np.array([0.,20.,0.]))
    glColor3ub(0, 0, 255)
    glVertex3fv(np.array([0.,0,20.]))
    glVertex3fv(np.array([0.,0.,-20.]))

    #white line
    glColor3ub(255,255,255)
    for i in range(-20, 21):
        if i != 0:
            glVertex3fv(np.array([20,0,i]))
            glVertex3fv(np.array([-20.,0,i]))
            glVertex3fv(np.array([i,0.,20]))
            glVertex3fv(np.array([i,0.,-20]))
    glEnd()
def drawCube():
    glBegin(GL_QUADS)
    glVertex3f( 0.1, 0.1,-0.1)                                                  
    glVertex3f(-0.1, 0.1,-0.1)                                                  
    glVertex3f(-0.1, 0.1, 0.1)                                                  
    glVertex3f( 0.1, 0.1, 0.1)                                                  
    glVertex3f( 0.1,-0.1, 0.1)                                                  
    glVertex3f(-0.1,-0.1, 0.1)                                                  
    glVertex3f(-0.1,-0.1,-0.1)                                                  
    glVertex3f( 0.1,-0.1,-0.1)                                                  
    glVertex3f( 0.1, 0.1, 0.1)                                                  
    glVertex3f(-0.1, 0.1, 0.1)                                                  
    glVertex3f(-0.1,-0.1, 0.1)                                                  
    glVertex3f( 0.1,-0.1, 0.1)                                                  
    glVertex3f( 0.1,-0.1,-0.1)                                                  
    glVertex3f(-0.1,-0.1,-0.1)                                                  
    glVertex3f(-0.1, 0.1,-0.1)                                                  
    glVertex3f( 0.1, 0.1,-0.1)                                                  
    glVertex3f(-0.1, 0.1, 0.1)                                                  
    glVertex3f(-0.1, 0.1,-0.1)                                                  
    glVertex3f(-0.1,-0.1,-0.1)                                                  
    glVertex3f(-0.1,-0.1, 0.1)                                                  
    glVertex3f( 0.1, 0.1,-0.1)                                                  
    glVertex3f( 0.1, 0.1, 0.1)                                                  
    glVertex3f( 0.1,-0.1, 0.1)                                                  
    glVertex3f( 0.1,-0.1,-0.1) 
    glEnd()

def drawTriangle():
    glBegin(GL_TRIANGLES)
    glVertex3fv(np.array([.0,.5,0.]))
    glVertex3fv(np.array([.0,.0,0.]))
    glVertex3fv(np.array([.5,.0,0.]))
    glEnd()

def drawSphere(rad,numLats=12, numLongs=12):
    for i in range(0, numLats + 1):
        lat0 = np.pi * (-0.5 + float(float(i - 1) / float(numLats)))
        z0 = np.sin(lat0)
        zr0 = np.cos(lat0)
        
        lat1 = np.pi * (-0.5 + float(float(i) / float(numLats)))
        z1 = np.sin(lat1)
        zr1 = np.cos(lat1)
        
        # Use Quad strips to draw the sphere
        glBegin(GL_QUAD_STRIP)
        
        for j in range(0, numLongs + 1):
            lng = 2 * np.pi * float(float(j - 1) / float(numLongs))
            x = np.cos(lng)
            y = np.sin(lng)
            glVertex3f(rad * x * zr0, rad * y * zr0, rad * z0)
            glVertex3f(rad * x * zr1, rad * y * zr1, rad * z1)
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

    while not glfw.window_should_close(window):
        glfw.poll_events()
        render()
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
