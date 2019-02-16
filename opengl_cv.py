# Skeleton Code for CS290I Homework 1
# 2012, Jon Ventura and Chris Sweeney

# adapt the include statements for your system:

import cv2 as cv
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import Pillow
import sys


cap = cv.VideoCapture(0)
image = 0
width = 640
height = 480

# a useful function for displaying your coordinate system
def drawAxes(length):

    glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT)

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glDisable(GL_LIGHTING)

    glBegin(GL_LINES)
    glColor3f(1,0,0)
    glVertex3f(0,0,0)
    glVertex3f(length,0,0)

    glColor3f(0,1,0)
    glVertex3f(0,0,0)
    glVertex3f(0,length,0)

    glColor3f(0,0,1)
    glVertex3f(0,0,0)
    glVertex3f(0,0,length)
    glEnd()


    glPopAttrib()


def display():
    global image
    global cap

    # clear the window
    glClear( GL_COLOR_BUFFER_BIT )

    # show the current camera frame

    #based on the way cv::Mat stores data, you need to flip it before displaying it
    tempimage = image.clone()
    cv.flip(image, tempimage, 0)
    glDrawPixels( tempimage.size().width, tempimage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempimage )

    #########################################
    # Here, set up new parameters to render a scene viewed from the camera.

    #set viewport
    glViewport(0, 0, tempimage.size().width, tempimage.size().height)

    #set projection matrix using intrinsic camera params
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()

    #gluPerspective is arbitrarily set, you will have to determine these values based
    #on the intrinsic camera parameters
    gluPerspective(60, tempimage.size().width*1.0/tempimage.size().height, 1, 20)

    #you will have to set modelview matrix using extrinsic camera params
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0)


    ########################################/
    # Drawing routine

    #now that the camera params have been  set, draw your 3D shapes
    #first, save the current matrix
    glPushMatrix()
    #move to the position where you want the 3D object to go
    glTranslatef(0, 0, 0) #this is an arbitrary position for demonstration
    #you will need to adjust your transformations to match the positions where
    #you want to draw your objects(i.e. chessboard center, chessboard corners)
    glutSolidTeapot(0.5)
    #glutSolidSphere(.3, 100, 100)
    drawAxes(1.0)
    glPopMatrix()


    # show the rendering on the screen
    glutSwapBuffers()

    # post the next redisplay
    glutPostRedisplay()


def reshape(w, h):

  # set OpenGL viewport (drawable area)
    glViewport(0, 0, w, h)


def mouse(button, state, x, y):
    if (button == GLUT_LEFT_BUTTON and state == GLUT_UP):
        pass





def keyboard(key, x, y ):

    if key == 'q':
      # quit when q is pressed
      sys.exit()



def idle():
    global image
    global cap
    # grab a frame from the camera
    image =  Pillow.open("checkerboardPhotos/2019-02-08-095838.jpg")


def main():
    if ( len(sys.argv) == 1 ):
      # start video capture from camera
        cap = cv.VideoCapture(0)
    elif ( len(sys.argv) == 2 ):
      # start video capture from file
        cap = cv.VideoCapture(sys.argv[1])
    else:
        print( "usage: %s [<filename>]\n", sys.argv[0] )
        return 1

    # check that video is opened
    if ( cap == None or not cap.isOpened() ):
        print( "could not start video capture\n" )
        return 1


    # get width and height
    w = int( cap.get( cv.CAP_PROP_FRAME_WIDTH ))
    h = int( cap.get( cv.CAP_PROP_FRAME_HEIGHT ))
    # On inu( there is currently a bug in OpenCV that returns
    # zero for both width and height here (at least for video from file)
    # hence the following override to global variable defaults:
    width = w if w else width
    height = h if h else height

    # initialize GLUT
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_RGBA or GLUT_DOUBLE)
    glutInitWindowPosition(20, 20)
    glutInitWindowSize(width, height)
    glutCreateWindow("OpenGL / OpenCV Example")

    # set up GUI callback functions
    glutDisplayFunc(display)
    glutReshapeFunc(reshape)
    glutMouseFunc(mouse)
    glutKeyboardFunc(keyboard)
    glutIdleFunc(idle)

    # start GUI loop
    glutMainLoop()



    return 0

main()
