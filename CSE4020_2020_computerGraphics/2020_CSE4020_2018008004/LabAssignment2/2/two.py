###################################################
# I used code which was provided by professor in lecture class. "2-code.py"
import glfw
from OpenGL.GL import *
import numpy as np
import math

def render(n):
    x_cord = np.cos(np.linspace(0, math.radians(360), 13))
    y_cord = np.sin(np.linspace(0, math.radians(360), 13))
    primitive_type_mod = [GL_POLYGON, GL_POINTS, GL_LINES, GL_LINE_STRIP, 
                          GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, 
                          GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP ]
    
    glClear(GL_COLOR_BUFFER_BIT)
    glLoadIdentity()
    glBegin(primitive_type_mod[n])
    for i in range(12):
        glVertex2f(x_cord[i], y_cord[i])
    glEnd()

# key pressing event calls render function
def key_callback(window, key, scancode, action, mods):    
    if key >= glfw.KEY_0 and key <= glfw.KEY_9:
        render(key - glfw.KEY_0)


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
    # initial render call for beginning
    render(4)
    # Loop until the user closes the window
    while not glfw.window_should_close(window):
        # Poll events
        glfw.poll_events()

        # Swap front and back buffers
        glfw.swap_buffers(window)

    glfw.terminate()

if __name__ == "__main__":
    main()
