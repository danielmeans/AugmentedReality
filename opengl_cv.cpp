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
#include <math.h>
#include <cstdio>
#include <fstream>
using namespace std;
using namespace cv;



cv::VideoCapture *cap = NULL;
int POT_FLAG = 1;
int SPHERE_FLAG = 0;
int width = 640;
int height = 480;
const int chessCornersX = 8;
const int chessCornersY = 6;
Mat image;
Mat camera_matrix;
Mat dist_coeffs;

void readIntrinsicParams(){
string fname = "out_camera_data.xml";
FileStorage fs( fname, FileStorage::READ );
fs["camera_matrix"] >> camera_matrix;
fs["distortion_coefficients"] >> dist_coeffs;

}

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

void drawTeapot()
{
glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
glPushMatrix();
glTranslatef(2.0, -3.0, 0.0);
glColor3f(0.0,1.0,0.0);
glRotatef(90,1.0, 0., 0.);
glTranslatef(2, 1.5, 0.0);

glutWireTeapot(3.0);
glPopMatrix();
glPopAttrib();


}

void drawSphere(){

glPushAttrib(GL_POLYGON_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
glPushMatrix();
glColor3f(0.0, 0.0, 1.0);
glTranslatef(-1.0, 0.0, 0.0);

for(int i = 0; i < chessCornersY; i++){
    glPushMatrix();
    for (int j = 0; j < chessCornersX; j++){
        glTranslatef(1.0, 0.0, 0.0);
        glutSolidSphere(0.2, 20, 20);
    }
    glPopMatrix();

    glTranslatef(0.0, -1.0, 0.0);
}

glPopMatrix();
glPopAttrib();
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
  float fovy = 2 *atan(0.5 * height / camera_matrix.at<double>(1,1) ) * 180 / M_PI;
  float aspect_ratio = width * 1.0 / height;
  gluPerspective(fovy, aspect_ratio , 0.1, 700);

  //you will have to set modelview matrix using extrinsic camera params
  vector<Point2f> realCorners;      // The corners we found in the real image.
  vector<Point3f> virtualCorners;       // The corresponding corner positions for where the corners lie on the chess board (measured in virtual units).
  Size patternSize(8,6);
  Mat r_vec;                       // The calculated rotation of the chess board.
  Mat t_vec;                    // The calculated translation of the chess board.
  Mat rotation;
  Mat viewMatrix = Mat::zeros(4, 4, CV_64FC1);
  int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
  bool foundCorners;
  Mat glViewMatrix = Mat::zeros(4, 4, CV_64FC1);
  if (image.data){
  cvtColor(image, grayImage, COLOR_BGR2GRAY);
      // Try to find the chess board corners in the image.
  foundCorners = findChessboardCorners(grayImage, patternSize, realCorners, chessBoardFlags);
  buildChessboardCornerPoints(&virtualCorners, 1.0);
    // Compute the rotation / translation of the chessboard (the cameras extrinsic pramaters).

  const float chessBoardScale = 1.0;
  if (foundCorners){
  solvePnP(Mat(virtualCorners), Mat(realCorners), camera_matrix, dist_coeffs, r_vec, t_vec, false);
  Rodrigues(r_vec, rotation);

  for (int row = 0; row < 3; ++row){
    for (int col = 0; col < 3; ++col) {
        viewMatrix.at<double>(row, col) = rotation.at<double>(row, col);
    }
    viewMatrix.at<double>(row, 3) = t_vec.at<double>(row,0);
  }
  viewMatrix.at<double>(3,3) = 1.0f;
  Mat cvToGl = Mat::zeros(4,4,CV_64F);
  cvToGl.at<double>(0,0) = 1.0f;
  cvToGl.at<double>(1,1) = -1.0f;
  cvToGl.at<double>(2,2) = -1.0f;
  cvToGl.at<double>(3,3) = 1.0f;
  viewMatrix = cvToGl * viewMatrix;

  transpose(viewMatrix, glViewMatrix);
  }
  }

  // If we weren’t able to find the corners exit early.


  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glLoadIdentity();

  //gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

  if(foundCorners){
   glLoadMatrixd(&glViewMatrix.at<double>(0,0));
  }
  glScalef(1.0, -1.0, -1.0);


  /////////////////////////////////////////////////////////////////////////////////
  // Drawing routine

  //now that the camera params have been set, draw your 3D shapes
  //first, save the current matrix
    //move to the position where you want the 3D object to go
    //glTranslatef(1.0, 1.0, 0.0); //this is an arbitrary position for demonstration
    //you will need to adjust your transformations to match the positions where
    //you want to draw your objects(i.e. chessboard center, chessboard corners)
    //drawAxes(1.0);
    if(POT_FLAG){
      drawTeapot();
    }
    if (SPHERE_FLAG){
      drawSphere();
    }
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
    case ' ':
      SPHERE_FLAG = (SPHERE_FLAG + 1) % 2;
      POT_FLAG = (POT_FLAG + 1) % 2;
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
  readIntrinsicParams();

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
