#include "glut.h"

void reshape(int w, int h)
{
    // Establish viewing area to cover entire window
    glViewport(0, 0, w, h);
}

void idle()
{
    glutPostRedisplay();
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  /* glBegin(GL_TRIANGLES); */
    /* glColor3f(0.0, 0.0, 1.0);  [> blue <] */
    /* glVertex2i(0, 0); */
    /* glColor3f(0.0, 1.0, 0.0);  [> green <] */
    /* glVertex2i(200, 200); */
    /* glColor3f(1.0, 0.0, 0.0);  [> red <] */
    /* glVertex2i(20, 200); */
  /* glEnd(); */

  glutSwapBuffers();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(640*2, 480*2);
  glutCreateWindow("Triangle");

  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);

  glClearColor(0.2f, 0.5f, 0.92f, 1.0f);

  glutMainLoop();

  return 0;
}
