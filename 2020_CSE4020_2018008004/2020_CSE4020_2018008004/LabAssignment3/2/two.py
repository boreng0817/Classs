###################################################
import glfw
from OpenGL.GL import *
import numpy as np
import math

# Global variable
g_CompsedM = np.identity(3)

def render(T):
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    # draw cooridnate
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([1.,0.]))
    glColor3ub(0, 255, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([0.,1.]))
    glEnd()
    # draw triangle
    glBegin(GL_TRIANGLES)
    glColor3ub(255, 255, 255)
    glVertex2fv( (T @ np.array([.0,.5,1.]))[:-1] )
    glVertex2fv( (T @ np.array([.0,.0,1.]))[:-1] )
    glVertex2fv( (T @ np.array([.5,.0,1.]))[:-1] )
    glEnd()

def scale_to_x(m):
    M = np.identity(3)
    M[0][0] = m
    return M

def rotate_degree(deg):
    R = np.identity(3)
    th = math.radians(deg)

    R[0:2, 0:2] = np.array([[np.cos(th), -np.sin(th)],
                            [np.sin(th),  np.cos(th)]])
    return R

def shear_to_x(m):
    M = np.identity(3)
    M[0][1] = m
    return M

def reflect_to_x_axis():
    M = np.identity(3)
    M[1][1] = -1
    return M

def apply_mat(M):
    global g_CompsedM
    g_CompsedM = M @ g_CompsedM

def pressed_or_repeat(action):
    return action==glfw.PRESS or action==glfw.REPEAT

def key_callback(window, key, scancode, action, mods):
    global g_CompsedM

    if key == glfw.KEY_W and pressed_or_repeat(action):
        apply_mat(scale_to_x(0.9))

    elif key == glfw.KEY_E and pressed_or_repeat(action):
        apply_mat(scale_to_x(1.1))

    elif key == glfw.KEY_S and pressed_or_repeat(action):
        apply_mat(rotate_degree(10))

    elif key == glfw.KEY_D and pressed_or_repeat(action):
        apply_mat(rotate_degree(-10))
    
    elif key == glfw.KEY_X and pressed_or_repeat(action):
        apply_mat(shear_to_x(-0.1))

    elif key == glfw.KEY_C and pressed_or_repeat(action):
        apply_mat(shear_to_x(0.1))

    elif key == glfw.KEY_R and pressed_or_repeat(action):
        apply_mat(reflect_to_x_axis())

    elif key == glfw.KEY_1:
        g_CompsedM = np.identity(3)



def main():
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

        render(g_CompsedM);


        # Swap front and back buffers
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
