Homework 2
Augmented and Mixed Reality
CS291A
2/7/19

To run, first build the executables:

1. Cmake .
2. make

To calibrate the camera based on the images provided with the
assignment enter "./calibrate" into the terminal. The output will
be "out_camera_data.xml" which is an xml file containing the
intrinsic parameters among other parameters.

To render the teapot/spheres atop of a chessboard using a webcam,
simply enter "./opengl_cv". Note that pressing the space key will
alternate from rendering the teapot and the spheres.

To use a video file as input, simply enter "./opengl_cv 'path/to/file'"
into the terminal. This should render the teapot/spheres onto the
video.

Sources used:
OpenCV Camera Calibration Tutorial
https://docs.opencv.org/4.0.1/d4/d94/tutorial_camera_calibration.html

OpenCV Forum on using SolvePnP
http://answers.opencv.org/question/23089/opencv-opengl-proper-camera-pose-using-solvepnp/


Given photos parameters:
Horizontal FoV: 59.9635858 rad
Vertical FoV: 46.7957864 rad
fx: 5.5466323243805505e+02
fy: 5.5466323243805505e+02
cx: 320
cy: 240
k1: -1.1347765800470654e-01
k2: 4.1286389924913480e-01
k3: 0
p1: 0
p2: -8.0739418237930738e-01


My Photos parameters:

Horizontal FoV: 50.503761 rad
Vertical FoV: 38.9627777 rad
fx: 6.7843937002454550e+02
fy: 6.7843937002454550e+02
cx: 320
cy: 240
k1: 1.1411086307264973e-01
k2: -1.0839201193011372e+00
k3: 0
p1: 0
p2: 2.4426064298877277e+00


Given photos errors:
Re-projection error: 1.25992
avg re projection error: 1.25992


My photos errors:
Re-projection error: 0.942798
 avg re projection error: 0.942798


My camera seems to have a a smaller field of view, both measured horizontally and vertically, compared to the camera used in the given photos. My camera has less error than the camera in the given photos.
The larger field of view of the other camera might add some distortion and reprojection error.  
