#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>

static void init(void);
static void draw(void);
static void sortie(void);

extern GLfloat _dim[];

extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);

static GLuint _gridId = 0;
static GLuint _pId = 0;
static GLfloat _speed = 3.0f;

void flyingTrip(int state) {
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
  initNoiseTextures();
  _gridId = gl4dgGenGrid2df(_dim[0] / 5, _dim[1] / 5);
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/flyingTrip.vs", "<fs>shaders/flyingTrip.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void draw(void) {
  float time = gl4dGetElapsedTime() / 1000.0f;
  
  /*
   * Faire varier la vue de gauche à droite avec : 
   * sin(vitesse du temps) * amplitude du mouvement
   */
  float xOffset = sinf(time) * 0.2f;
  /* Faire varier l'inclinaison entre 0 et 10 degrés */
  float rollAngle = sinf(time) * 10.0f;

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId);

  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _dim[1]) / _dim[0];
  GLfloat top    = (1.0f * _dim[1]) / _dim[0];
  gl4duFrustumf(-1.0f * 0.1f, 1.0f * 0.1f, bottom * 0.1f, top * 0.1f, 0.1f, 100.0f);

  glUniform1f(glGetUniformLocation(_pId, "time"), time);
  glUniform1f(glGetUniformLocation(_pId, "speed"), _speed);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duRotatef(rollAngle, 1.0f, 0.0f, 0.0f); 

  gl4duLookAtf(
    xOffset, 1.0f, 1.0f,
    xOffset, 0.7f, 0.0f,
    0.0f, 1.0f, 0.0f
  );
  gl4duScalef(7.0f, 1.0f, 6.0f);
  gl4duSendMatrices();
  
  useNoiseTextures(_pId, 0);
  gl4dgDraw(_gridId);
  unuseNoiseTextures(0);
  
  glUseProgram(0);
}

void sortie(void) {
  freeNoiseTextures();
}
