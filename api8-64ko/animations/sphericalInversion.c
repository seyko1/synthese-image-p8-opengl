#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>

static void init(void);
static void draw(void);
static void sortie(void);

extern GLfloat _dim[];

static GLuint _quadId = 0;
static GLuint _pId = 0;

void sphericalInversion(int state) {
  switch(state) {
    case GL4DH_INIT:
      init();
      return;
    case GL4DH_FREE:
      sortie();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      return;
    default:
      draw();
      return;
  }
}

void init(void) {
  _pId = gl4duCreateProgram("<vs>shaders/sphericalInversion.vs", "<fs>shaders/sphericalInversion.fs", NULL);
  _quadId = gl4dgGenQuadf();
  
  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "sphericalProj");
  
  gl4duBindMatrix("sphericalProj");
  gl4duLoadIdentityf();
  
  gl4duOrthof(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
}

void draw(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -1.0f);
  gl4duSendMatrices();

  float t = gl4dGetElapsedTime() / 1000.0f;
  glUniform1f(glGetUniformLocation(_pId, "elapsed"), t);
  glUniform2f(glGetUniformLocation(_pId, "resolution"), _dim[0], _dim[1]);

  gl4dgDraw(_quadId);

  glUseProgram(0);
}

void sortie(void) {
 }
