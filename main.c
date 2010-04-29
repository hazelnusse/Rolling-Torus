#include <GL/glut.h>
#include <gsl/gsl_odeiv.h>
#include <stdio.h>
#include <math.h>
#include <png.h>
#include "torus.h"

#define FRAME_RATE 120
#define TIME 10
#define SCALE 1
#define WIDTH 1280
#define HEIGHT 720
#define G 9.81
#define R1 0.594
#define R2 0.055
#define YAW_I (M_PI/4.0)
#define LEAN_I 0.1
#define SPIN_I 0.0
#define X_I 0.0
#define Y_I 0.0
#define U1_I 0.1
#define U2_I 1.5
#define U3_I -0.1

double COx[FRAME_RATE*TIME + 1];
double COy[FRAME_RATE*TIME + 1];
double COz[FRAME_RATE*TIME + 1];
double CNx[FRAME_RATE*TIME + 1];
double CNy[FRAME_RATE*TIME + 1];
double yaw[FRAME_RATE*TIME + 1];
double lean[FRAME_RATE*TIME + 1];
double spin[FRAME_RATE*TIME + 1];

int k = 0;                     // global for index

void init(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glDisable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
}

int SavePicture(){
    int i;
    png_bytep *row_pointers;
    unsigned char *buffer;
    char filename[50];
	png_structp png_ptr;
	png_infop info_ptr;
    FILE *fp;
    sprintf(filename, "./pngs/torus%04d.png", k);
	
	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "capture: Couln't open output file \"%s\"", filename);
        return 1;
    }

    buffer = (unsigned char *) malloc(WIDTH * HEIGHT * 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0,0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    /*  Initialize PNG structs */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
    if (!png_ptr) {
        fprintf(stderr, "capture: Can't initialize png_ptr");
        return 1;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
         png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
         fprintf(stderr, "capture: Can't initialze info_ptr");
         return 1;
    }

    /*  Initialize PNG error jump */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        fprintf(stderr, "capture: Unknown error");
        return 1;
    }

    /*  Give PNG the file handle */
    png_init_io(png_ptr, fp);

    /*  Set PNG options/info.  Assumes 24 bit color buffer, change this
     *  if you need to.
     */
    png_set_IHDR(png_ptr, info_ptr, 
        WIDTH,                          /* Width */
        HEIGHT,                         /* Height */
        8,                              /* Bit depth */ 
        PNG_COLOR_TYPE_RGB_ALPHA,       /* Color type */
        PNG_INTERLACE_NONE,             /* Interlacing */
        PNG_COMPRESSION_TYPE_DEFAULT,   /* Compression */
        PNG_FILTER_TYPE_DEFAULT);       /* Filter method */
    
    /*  Set up row pointers.  OpenGL stores the buffer in reverse
     *  row order to PNG (bottom-to-top instead of top-to-bottom),
     *  so we flip them here.
     */
    row_pointers = png_malloc(png_ptr, HEIGHT * sizeof(png_bytep));
    for (i = 0; i< HEIGHT; i++)
        row_pointers[i] = &buffer[(HEIGHT - i - 1) * WIDTH* 4];
    png_set_rows(png_ptr, info_ptr, row_pointers);
    
    /*  Write the PNG */
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /*  Free up */
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
}

void display(void)
{
  int i;
  glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  gluLookAt(CNx[0], CNy[0], -8.0,   // camera position
            CNx[0], CNy[0], 0.0,   // point camera at this position
             1.0, 0.0, 0.0);  // define up of the camera

  //Add ambient light
  GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f}; //Color (0.2, 0.2, 0.2)
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  //Add positioned light
  GLfloat lightColor0[] = {0.7f, 0.7f, 0.7f, 1.0f}; //Color (0.5, 0.5, 0.5)
  GLfloat lightPos0[] = {0.0f, 0.0f, -2.0f, 1.0f}; //Positioned at (4, 0, 8)
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

  //Add directed light
  GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
  //Coming from the direction (-1, 0.5, 0.5)
  GLfloat lightPos1[] = {-1.0f, 0.0f, 0.0f, 0.0f};
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
  glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

  // Draw path
  glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < k; ++i)
      glVertex3f(CNx[i], CNy[i], 0.0);
    glEnd();
  glPopMatrix();
      

  // x axis
  glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
  glEnd();

  glPushMatrix();
    glTranslatef(0.9, 0.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glutWireCone(0.05, 0.1, 10, 1);
  glPopMatrix();

  // y axis
  glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
  glEnd();

  glPushMatrix();
    glTranslatef(0.0, 0.9, 0.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glutWireCone(0.05, 0.1, 10, 1);
  glPopMatrix();

  // z axis
  glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
  glEnd();

  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.9);
    glutWireCone(0.05, 0.1, 10, 1);
  glPopMatrix();

  // Draw Torus
  glPushMatrix();
    glColor3f(0.9, 0.2, .7);
    glTranslatef(COx[k], COy[k], COz[k]);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glRotatef(yaw[k], 0.0, -1.0, 0.0);
    glRotatef(lean[k], 1.0, 0.0, 0.0);
    glRotatef(spin[k], 0.0, 0.0, 1.0);
    glutSolidTorus(R2, R1, 30, 30);
  glPopMatrix();


  glFinish();
  SavePicture();  //save pixels before swapping buffer
  glutSwapBuffers();  // Only needed if in double buffer mode
}

void reshape (int w, int h)
{
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective(45.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard (unsigned char key, int x, int y)
{
  if (key == 27)
    exit(0);
}

void updateState(int value)
{
  glutPostRedisplay();
  ++k;
  if (k == FRAME_RATE*TIME)
    exit(0);
  // re-register the callback
  glutTimerFunc((unsigned int) (SCALE * 1000.0/ (double) FRAME_RATE), updateState, 0);  // update 60 times per second
}

int main(int argc, char** argv)
{
  double state[8] = {YAW_I, LEAN_I, SPIN_I, X_I, Y_I, U1_I, U2_I, U3_I};
  int j;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rkf45;
  double params[3] = {G, R1, R2};
  double t = 0.0, tj;
  double h = 1e-3;


  for (j = 0; j < 8; ++j) {
    yaw[0] = (180./M_PI*state[0] > 360) ? 180./M_PI*state[0] - 360. : 180./M_PI*state[0];
    lean[0] = (180./M_PI*state[1] > 360) ? 180./M_PI*state[1] - 360. : 180./M_PI*state[1];
    spin[0] = (180./M_PI*state[2] > 360) ? 180./M_PI*state[2] - 360. : 180./M_PI*state[2];
    COx[0] = -R1*sin(state[0])*sin(state[1]) + state[3];
    COy[0] =  R1*cos(state[0])*sin(state[1]) + state[4];
    COz[0] = -R2 - R1*cos(state[1]);
    CNx[0] = state[3];
    CNy[0] = state[4];
  }

  // Set up the integration
  gsl_odeiv_step * s = gsl_odeiv_step_alloc (T, 8);
  gsl_odeiv_control * c = gsl_odeiv_control_y_new (1e-6, 0.0);
  gsl_odeiv_evolve * e = gsl_odeiv_evolve_alloc(8);
  gsl_odeiv_system sys = {TorusEOMS, NULL, 8, params};

  // Do the integration
  for (j = 1; j < FRAME_RATE*TIME + 1; ++j) {
    tj = j / (double) FRAME_RATE;
    while (t < tj)
      gsl_odeiv_evolve_apply (e, c, s, &sys, &t, tj, &h, state);

    yaw[j] = (180./M_PI*state[0] > 360) ? 180./M_PI*state[0] - 360. : 180./M_PI*state[0];
    lean[j] = (180./M_PI*state[1] > 360) ? 180./M_PI*state[1] - 360. : 180./M_PI*state[1];
    spin[j] = (180./M_PI*state[2] > 360) ? 180./M_PI*state[2] - 360. : 180./M_PI*state[2];
    COx[j] = -R1*sin(state[0])*sin(state[1]) + state[3];
    COy[j] =  R1*cos(state[0])*sin(state[1]) + state[4];
    COz[j] = -R2 - R1*cos(state[1]);
    CNx[j] = state[3];
    CNy[j] = state[4];

  } // for j

  // Free allocated GSL variables
  gsl_odeiv_evolve_free(e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free(s);

  // Initialize animation window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("Rolling Torus Animation");
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc((unsigned int) (SCALE * 1000.0/ (double) FRAME_RATE), updateState, 0);
  glutMainLoop();
  return 0;
}
