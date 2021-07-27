import glfw
from OpenGL.GL import *
import numpy as np

# Given function 1
def drawFrame():
    glBegin(GL_LINES)
    glColor3ub(255, 0, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([1.,0.]))
    glColor3ub(0, 255, 0)
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([0.,1.]))
    glEnd()

# Given function 2
def drawTriangle():
    glBegin(GL_TRIANGLES)
    glVertex2fv(np.array([0.,.5]))
    glVertex2fv(np.array([0.,0.]))
    glVertex2fv(np.array([.5,0.]))
    glEnd()

def render():
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()

    # Draw global frame and white triangle
    drawFrame()
    glColor3ub(255, 255, 255)
    drawTriangle()

    # Rotate pi/6 and translate 0.6 by x-axis
    # w.r.t to global frame.
    glTranslatef(.6, 0, 0)
    glRotatef(30, 0, 0, 1)

    # Draw local frame, and blue triangle
    drawFrame()
    glColor3ub(0, 0, 255)
    drawTriangle()

    
def main():
    # Initialize the library
    if not glfw.init():
        return
    # Create a windowed mode window and its OpenGL context
    window = glfw.create_window(480,480,"2018008004", None, None)
    if not window:
        glfw.terminate()
        return
    
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
