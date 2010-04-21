#include <GL/glut.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv.h>
#include <stdio.h>
#include <math.h>
#include "torus.h"

#define FRAME_RATE 60
#define TIME 2
#define G 9.81
#define R1 0.594
#define R2 0.025
#define SCALE 1

double x[8][FRAME_RATE*TIME];  // global for state
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

  gluLookAt(x[3][k]-5.0, x[4][k], 0.0,   // camera position
            x[3][k], x[4][k], 0.0,   // point camera at this position
            0.0, 0.0, -1.0);  // define up of the camera

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
    glTranslatef(-R1*sin(x[0][k])*sin(x[1][k]) + x[3][k],
                  R1*cos(x[0][k])*sin(x[1][k]) + x[4][k],
                  -R2 - R1*cos(x[1][k]));
    glRotatef(90.0, 1.0, 0.0, 0.0);  // puts the torus upright
    glRotatef(x[0][k]*180./M_PI, 0.0, 0.0, 1.0);
    glRotatef(x[1][k]*180./M_PI, 1.0, 0.0, 0.0);
    glRotatef(x[2][k]*180./M_PI, 0.0, 1.0, 0.0);
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
  double state[8] = {M_PI/4., 0.0, 0.0, 1.0, 1.0, -1.75, 9.0, 0.0};
  int i, j;
  const gsl_odeiv_step_type * T = gsl_odeiv_step_rk8pd;
  double params[3] = {G, R1, R2};
  double t = 0.0, tj;
  double h = 1e-3;

  for (i = 0; i < 8; ++i)
    x[i][0] = state[i];

  printf("%5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f\n",
              t,  state[0],   state[1],   state[2],   state[3],   state[4],   state[5],   state[6],   state[7]);
  gsl_odeiv_step * s = gsl_odeiv_step_alloc (T, 8);
  gsl_odeiv_control * c = gsl_odeiv_control_y_new (1e-6, 0.0);
  gsl_odeiv_evolve * e = gsl_odeiv_evolve_alloc(8);
  gsl_odeiv_system sys = {TorusEOMS, NULL, 8, params};

  for (j = 1; j <= FRAME_RATE*TIME; ++j) {
    tj = j / (double) FRAME_RATE;
    while (t < tj)
      gsl_odeiv_evolve_apply (e, c, s, &sys, &t, tj, &h, state);
    for (i = 0; i < 8; ++i)
      x[i][j] = state[i];

  printf("%5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f | %5.5f\n",
              t,  state[0],   state[1],   state[2],   state[3],   state[4],   state[5],   state[6],   state[7]);
  } // for
  gsl_odeiv_evolve_free(e);
  gsl_odeiv_control_free(c);
  gsl_odeiv_step_free(s);


  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Rolling Torus Animation");
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc((unsigned int) (SCALE * 1000.0/ (double) FRAME_RATE), updateState, 0);
  glutMainLoop();
  return 0;
}
