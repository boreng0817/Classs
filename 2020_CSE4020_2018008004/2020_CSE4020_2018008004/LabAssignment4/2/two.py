import glfw
from OpenGL.GL import *
import numpy as np

def render(M):
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
    glColor3ub(255, 255, 255)
    
    # draw point p
    glBegin(GL_POINTS)
    T = np.identity(3)
    T[0][2] = .5
    glVertex2fv( ((M @ T) @ np.array([.5,.0,1.]))[:-1] )
    glEnd()
    
    # draw vector v
    glBegin(GL_LINES)
    glVertex2fv( (M @ np.array([.0,.0,1.]))[:-1] )
    glVertex2fv( (M @ np.array([.5,.0,1.]))[:-1] )
    glEnd()


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
        t = glfw.get_time()
        th = t
        #Rotation matrix
        r = ([[np.cos(th), -np.sin(th)],
              [np.sin(th),  np.cos(th)]])
        R = np.identity(3)
        R[0:2, 0:2] = r

        render(R);


        # Swap front and back buffers
        glfw.swap_buffers(window)
    glfw.terminate()

if __name__ == "__main__":
    main()
