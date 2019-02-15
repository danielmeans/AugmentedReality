// Skeleton Code for CS290I Homework 1
// 2012, Jon Ventura and Chris Sweeney

// adapt the include statements for your system:

#include <opencv2/opencv.hpp>

#include <opencv2/highgui/highgui_c.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <cstdio>
#include <fstream>
using namespace std;
using namespace cv;



cv::VideoCapture *cap = NULL;
int width = 640;
int height = 480;
const int chessCornersX = 8;
const int chessCornersY = 6;
cv::Mat image;

void buildChessboardCornerPoints(std::vector<Point3f>* corners, float scale)
{
  if (corners != nullptr)
  {
    for (int ix = 0; ix < chessCornersY; ix++)
    {
      for (int iy = 0; iy < chessCornersX; iy++)
      {
        corners->push_back(Point3f(iy*scale, ix*scale, 0.0));
      }
    }
  }
}

// a useful function for displaying your coordinate system
void drawAxes(float length)
{
  glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT) ;

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) ;
  glDisable(GL_LIGHTING) ;

  glBegin(GL_LINES) ;
  glColor3f(1,0,0) ;
  glVertex3f(0,0,0) ;
  glVertex3f(length,0,0);

  glColor3f(0,1,0) ;
  glVertex3f(0,0,0) ;
  glVertex3f(0,length,0);

  glColor3f(0,0,1) ;
  glVertex3f(0,0,0) ;
  glVertex3f(0,0,length);
  glEnd() ;


  glPopAttrib() ;
}

void display()
{
  // clear the window
  glClear( GL_COLOR_BUFFER_BIT );

  // show the current camera frame

  //based on the way cv::Mat stores data, you need to flip it before displaying it
  cv::Mat tempimage;
  cv::Mat grayImage;
  cv::flip(image, tempimage, 0);
  glDrawPixels( tempimage.size().width, tempimage.size().height, GL_BGR, GL_UNSIGNED_BYTE, tempimage.ptr() );

  //////////////////////////////////////////////////////////////////////////////////
  // Here, set up new parameters to render a scene viewed from the camera.

  //set viewport
  glViewport(0, 0, tempimage.size().width, tempimage.size().height);

  //set projection matrix using intrinsic camera params
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //gluPerspective is arbitrarily set, you will have to determine these values based
  //on the intrinsic camera parameters
  gluPerspective(49.535321507097954, 1.348276238847116, 0.5, 500);

  //you will have to set modelview matrix using extrinsic camera params
  vector<Point2f> realCorners;      // The corners we found in the real image.
  vector<Point3f> virtualCorners;       // The corresponding corner positions for where the corners lie on the chess board (measured in virtual units).
  Size patternSize(8,6);
  Mat rotation;                       // The calculated rotation of the chess board.
  Mat translation;                    // The calculated translation of the chess board.
  int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;

  if (image.data){
    cvtColor(image, grayImage, COLOR_BGR2GRAY);
      // Try to find the chess board corners in the image.
  bool foundCorners = findChessboardCorners(grayImage, patternSize, realCorners, chessBoardFlags);
  buildChessboardCornerPoints(&virtualCorners, 1.0);
    // Compute the rotation / translation of the chessboard (the cameras extrinsic pramaters).

  const float chessBoardScale = 1.0;
  float focal_length = 635.5372342324713;
  float center_x = 345.9518885070029;
  float center_y = 286.7182067537758;
  float focal_height = 638.959818122316;
  cv::Mat camera_matrix = (Mat_<float>(3,3) << focal_length, 0, center_x, 0 , focal_height, center_y, 0, 0, 1);
  cv::Mat dist_coeffs = (Mat_<float>(5,1) << -0.019971260474681007, 0.4434525041243878, 0.009682201827214744, 0.004195454828454007, -2.2207837789884732);
  //cout << fprintf(stderr, "%d", Mat(realCorners).checkVector(2, CV_32F));
  //cout << fprintf(stderr, "%d",Mat(realCorners).checkVector(2, CV_64F));
  if (foundCorners){
  solvePnP(Mat(virtualCorners), Mat(realCorners), camera_matrix, dist_coeffs, rotation, translation);
  }
  }

  // If we weren’t able to find the corners exit early.


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);  


  /////////////////////////////////////////////////////////////////////////////////
  // Drawing routine

  //now that the camera params have been set, draw your 3D shapes
  //first, save the current matrix
  glPushMatrix();
    //move to the position where you want the 3D object to go
    glTranslatef(0, 0, 0); //this is an arbitrary position for demonstration
    //you will need to adjust your transformations to match the positions where
    //you want to draw your objects(i.e. chessboard center, chessboard corners)
    glutSolidTeapot(0.5);
    //glutSolidSphere(.3, 100, 100);
    drawAxes(1.0);
  glPopMatrix();
  

  // show the rendering on the screen
  glutSwapBuffers();

  // post the next redisplay
  glutPostRedisplay();
}

void reshape( int w, int h )
{
  // set OpenGL viewport (drawable area)
  glViewport( 0, 0, w, h );
}

void mouse( int button, int state, int x, int y )
{
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {

    }
}

void keyboard( unsigned char key, int x, int y )
{
  switch ( key )
    {
    case 'q':
      // quit when q is pressed
      exit(0);
      break;

    default:
      break;
    }
}

void idle()
{
  // grab a frame from the camera
  (*cap) >> image;
}




//vector<float> readIntrinsicParams(){
//    ifstream infile;
//    infile.open("intrinsic_params.txt");
//    if (!infile) {
//    cerr << "Unable to open file datafile.txt";
//    exit(1);   // call system to stop
//    }
//    float a, b;
//    int i = 0;
//    vector<float> res;
//    while (infile >> a)
//    {
//        res[i] = a;
//        i++;
//    }
//    infile.close();
//    return res;
//    }

int main( int argc, char **argv )
{
  int w,h;

  if ( argc == 1 ) {
    // start video capture from camera
    cap = new cv::VideoCapture(0);
  } else if ( argc == 2 ) {
    // start video capture from file
    cap = new cv::VideoCapture(argv[1]);
  } else {
    fprintf( stderr, "usage: %s [<filename>]\n", argv[0] );
    return 1;
  }

  // check that video is opened
  if ( cap == NULL || !cap->isOpened() ) {
    fprintf( stderr, "could not start video capture\n" );
    return 1;
  }

  // get width and height
  w = (int) cap->get( cv::CAP_PROP_FRAME_WIDTH );
  h = (int) cap->get( cv::CAP_PROP_FRAME_HEIGHT );
  fprintf(stderr, "%d", w);
//  vector<float> res = readIntrinsicParams();
//  cout << res[0];

  // On Linux, there is currently a bug in OpenCV that returns 
  // zero for both width and height here (at least for video from file)
  // hence the following override to global variable defaults: 
  width = w ? w : width;
  height = h ? h : height;

  // initialize GLUT
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
  glutInitWindowPosition( 20, 20 );
  glutInitWindowSize( width, height );
  glutCreateWindow( "OpenGL / OpenCV Example" );

  // set up GUI callback functions
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc( mouse );
  glutKeyboardFunc( keyboard );
  glutIdleFunc( idle );

  // start GUI loop
  glutMainLoop();

  return 0;
}
