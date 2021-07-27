import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

gCamAng = 0
gComposedM = np.identity(4)

def get_translate(x):
    M = np.identity(4)
    M[0][3] = x
    return M

def get_rotate_x(deg):
    deg = np.radians(deg)
    M = np.identity(4)
    M[1:3, 1:3] = np.array([[np.cos(deg), -np.sin(deg)],
                            [np.sin(deg),  np.cos(deg)]])
    return M

def get_rotate_y(deg):
    deg = np.radians(deg)
    M = np.identity(4)
    M[0:3, 0:3] = np.array([[ np.cos(deg), 0, np.sin(deg)],
                            [           0, 1,           0],
                            [-np.sin(deg), 0, np.cos(deg)]])
    return M

def render(M, camAng):
    # enable depth test (we'll see details later)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glEnable(GL_DEPTH_TEST)
    
    glLoadIdentity()
    
    # use orthogonal projection (we'll see details later)
    glOrtho(-1,1, -1,1, -1,1)
    
    # rotate "camera" position to see this 3D space better (we'll see 
    # details later)
    gluLookAt(.1*np.sin(camAng),.1,.1*np.cos(camAng), 0,0,0, 0,1,0)
    
    # draw coordinate: x in red, y in green, z in blue
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex3fv(np.array([0.,0.,0.]))
    glVertex3fv(np.array([1.,0.,0.]))
    glColor3ub(0, 255, 0)
    glVertex3fv(np.array([0.,0.,0.]))
    glVertex3fv(np.array([0.,1.,0.]))
    glColor3ub(0, 0, 255)
    glVertex3fv(np.array([0.,0.,0]))
    glVertex3fv(np.array([0.,0.,1.]))
    glEnd()

    # draw triangle
    glBegin(GL_TRIANGLES)
    glColor3ub(255, 255, 255)
    glVertex3fv((M @ np.array([.0,.5,0.,1.]))[:-1])
    glVertex3fv((M @ np.array([.0,.0,0.,1.]))[:-1])
    glVertex3fv((M @ np.array([.5,.0,0.,1.]))[:-1])
    glEnd()

def key_callback(window, key, scancode, action, mods):
    global gCamAng, gComposedM
    if action==glfw.PRESS or action==glfw.REPEAT:

        if key==glfw.KEY_1:
            gCamAng += np.radians(-10)
        elif key==glfw.KEY_3:
            gCamAng += np.radians(10)

        elif key==glfw.KEY_Q:
            gComposedM = get_translate(-0.1) @ gComposedM
        elif key==glfw.KEY_E:
            gComposedM = get_translate(0.1) @ gComposedM
        
        elif key==glfw.KEY_A:
            gComposedM = gComposedM @ get_rotate_y(-10)
        elif key==glfw.KEY_D:
            gComposedM = gComposedM @ get_rotate_y(10)
        elif key==glfw.KEY_W:
            gComposedM = gComposedM @ get_rotate_x(-10)
        elif key==glfw.KEY_S:
            gComposedM = gComposedM @ get_rotate_x(10)
def main():
    global gCamAng, gComposedM
    # Initialize the library
    if not glfw.init():
        return
    # Create a windowed mode window and its OpenGL context
    window = glfw.create_window(480,480,"2018008004", None, None)
    if not window:
        glfw.terminate()
        return
    glfw.set_key_callback(window, key_callback)
    
    # Make the window's context current
    glfw.make_context_current(window)
    # Loop until the user closes the window
    while not glfw.window_should_close(window):
        # Poll events
        glfw.poll_events()
        render(gComposedM, gCamAng);
        # Swap front and back buffers
        glfw.swap_buffers(window)
    glfw.terminate()

if __name__ == "__main__":
    main()




