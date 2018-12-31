
#include <GL/glut.h>

void render();
void update();

void resize(int width, int height);

int main(int argc, char * argv[])
{
    glutInit(&argc, argv);

    glutInitWindowPosition(50, 50);

    glutInitWindowSize(720, 720);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Solar system visualization");

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(render);

    glutIdleFunc(update);

    glutReshapeFunc(resize);

    glutMainLoop();

    return 0;
}

static float angle = 30.0f;

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

    glPushMatrix();

    glTranslatef(0.0, 0.0, -50.0);

    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    glutSolidCube(5.0);

    glPopMatrix();

    glColor4f(1.0f, 0.0f, 1.0f, 0.2f);

    glPushMatrix();

    glTranslatef(0.0, 0.0, -50.0);

    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    glTranslatef(15.0, 0.0, 0.0);

    glutSolidCube(5.0);

    glPopMatrix();

    glutSwapBuffers();
}

void update()
{
    angle += (angle < 360.0f ? 1.0 : -360.0f);

    glutPostRedisplay();
}

void resize(int width, int height)
{
    if (!height)
        height = 1;

    const float ratio = static_cast<float>(width) / static_cast<float>(height);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glViewport(0, 0, width, height);

    gluPerspective(45, ratio, 20, 300);

    glMatrixMode(GL_MODELVIEW);
}
