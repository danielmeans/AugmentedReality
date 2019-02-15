PROJECT( opengl_cv )

cmake_minimum_required(VERSION 3.1)
# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

FIND_PACKAGE( OpenCV )
FIND_PACKAGE( OpenGL )
FIND_PACKAGE( GLU )
FIND_PACKAGE( GLUT )
SET(MySources opengl_cv.cpp )
SET(OpenCV_DIR /installation/OpenCV-/lib/cmake/opencv4)
ADD_EXECUTABLE( opengl_cv ${MySources} )
TARGET_LINK_LIBRARIES( opengl_cv ${OpenCV_LIBS} ${OpenGL_LIBRARY} ${GLU_LIBRARY} ${GLUT_LIBRARY} )


