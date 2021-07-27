import glfw
from OpenGL.GL import *
import numpy as np

key_pressed = ["0"]

def render():
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    # draw cooridnates
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([1.,0.]))
    glColor3ub(0, 255, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([0.,1.]))
    glEnd()
    glColor3ub(255, 255, 255)

    # implement here
    global key_pressed

    key_pressed.reverse()
    for s in key_pressed:
        if s == "Q":
            glTranslatef(-0.1, 0., 0.)
        elif s == "E":
            glTranslatef( 0.1, 0., 0.)
        elif s == "A":
            glRotatef( 10, 0, 0, 1)
        elif s == "D":
            glRotatef(-10, 0, 0, 1)
    key_pressed.reverse()
    drawTriangle()

def drawTriangle():
    glBegin(GL_TRIANGLES)
    glVertex2fv(np.array([0.,.5]))
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([.5,0.]))
    glEnd()

def pressed_or_repeat(action):
    return action==glfw.PRESS or action==glfw.REPEAT

def key_callback(window, key, scancode, action, mods):
    global key_pressed

    if key == glfw.KEY_Q and pressed_or_repeat(action):
        key_pressed.append("Q")

    elif key == glfw.KEY_E and pressed_or_repeat(action):
        key_pressed.append("E")

    elif key == glfw.KEY_A and pressed_or_repeat(action):
        key_pressed.append("A")

    elif key == glfw.KEY_D and pressed_or_repeat(action):
        key_pressed.append("D")
    
    elif key == glfw.KEY_1 and pressed_or_repeat(action):
        key_pressed = ["0"]


def main():
    global key_pressed
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
        render();


        # Swap front and back buffers
        glfw.swap_buffers(window)
    glfw.terminate()

if __name__ == "__main__":
    main()
