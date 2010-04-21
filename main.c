#include <GL/glut.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>
#include <stdio.h>
#include <math.h>
#include "torus.h"

#define FRAME_RATE 60
#define TIME 20 
#define G 9.81
#define R1 0.594
#define R2 0.055
#define SCALE 1

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
  glShadeModel(GL_FLAT);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();

  gluLookAt(CNx[1], CNy[1], -3.5,   // camera position
           CNx[1], CNy[1], 0.0,   // point camera at this position
           2.0, 0.0, -10.0);  // define up of the camera

  // x axis
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
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
  glBegin(GL_LINES);
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
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
  glEnd();

  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.9);
    glutWireCone(0.05, 0.1, 10, 1);
  glPopMatrix();

  glPushMatrix();
    glColor3f(0.5, 0.5, .5);
    glTranslatef(COx[k], COy[k], COz[k]);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(yaw[k], 0.0, 1.0, 0.0);
    glRotatef(lean[k], 1.0, 0.0, 0.0);
    glRotatef(spin[k], 0.0, 0.0, -1.0);
    glutWireTorus(R2, R1, 20, 20);
  glPopMatrix();

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
  ++k;
  if (k == FRAME_RATE*TIME)
    k = 0;
  glutPostRedisplay();
  // re-register the callback
  glutTimerFunc((unsigned int) (SCALE * 1000.0/ (double) FRAME_RATE), updateState, 0);  // update 60 times per second
}

int main(int argc, char** argv)
{
  double state[8] = {0.0, 0.1, 0.0, 1.0, 1.0, 0.0, 0.0, 3.0};
  int j;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rkf45;
  double params[3] = {G, R1, R2};
  double t = 0.0, tj;
  double h = 1e-3;

  printf("%5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | "
         "%5.5f\n", t,  state[0],   state[1],   state[2],   state[3],
          state[4],   state[5],   state[6],   state[7]);

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
    COx[j] = -R1*sin(M_PI/180.*state[0])*sin(M_PI/180.*state[1]) + state[3];
    COy[j] =  R1*cos(M_PI/180.*state[0])*sin(M_PI/180.*state[1]) + state[4];
    COz[j] = -R2 - R1*cos(M_PI/180.*state[1]);
    CNx[j] = state[3];
    CNy[j] = state[4];

    printf("%5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | "
           "%5.5f\n", t,  state[0],   state[1],   state[2],   state[3],
            state[4],   state[5],   state[6],   state[7]);
  } // for j

  // Free allocated GSL variables
  gsl_odeiv_evolve_free(e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free(s);


  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(900, 100);
  glutCreateWindow("Rolling Torus Animation");
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc((unsigned int) (SCALE * 1000.0/ (double) FRAME_RATE), updateState, 0);
  glutMainLoop();
  return 0;
}
