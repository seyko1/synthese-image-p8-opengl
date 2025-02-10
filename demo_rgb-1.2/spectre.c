#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>
#include "audioHelper.h"

#define NB_E 16

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static GLuint _wW = 640, _wH = 480;
static GLuint _cubeId = 0;
static GLuint _pId = 0;

static int _hauteurs[NB_E];

void spectre(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  static double mp = 0.0;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    int length = ahGetAudioStreamLength();
    short * stream = (short*) ahGetAudioStream();
    double m = 0.0;

    // Avancer d'un pas de deux short (pour du stréréo)
    for (int i = 0; i < length / 2; ++i) {
      m += fabs(stream[i] / (double) (1 << 15));
    }
    mp = m / (length / 2);
    printf("%f", mp);
    return;

    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}

void init(void) {
  int i;
  // Initialisation du tableau _hauteurs
  for (i = 0; i < NB_E; ++i) {
    // valeur aléatoire entre -128 et 127
    _hauteurs[i] = -128 + (rand() & 0xFF);
  }
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  _cubeId = gl4dgGenCubef();
  // Créer un programme shader à partir de hello.vs et hello.fs, qui pourra s'occuper du rendu.
  _pId = gl4duCreateProgram("<vs>shaders/spectre.vs", "<fs>shaders/spectre.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  // Appliquer une matrice de projection, elle sera envoyé lors de l'appel à sendMatrices()
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  GLfloat bottom = (-1.0f * _wH) / _wW;
  GLfloat top    = (1.0f * _wH) / _wW;
  gl4duFrustumf(-1.0f, 1.0f, bottom, top, 1.0f, 100.0f);
}

void draw(void) {
  int i, j;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf(); // matrice neutre utilisée pour l'initialisation 
  
  gl4duTranslatef(0.0, 0.0f, -1.1f);
  for (i = 0; i < NB_E; ++i) {
    // Ramener i dans la plage [-1, 1] 
    GLfloat x = 2.0f * (i / (NB_E - 1.0f)) - 1.0f;
    gl4duPushMatrix();
    gl4duTranslatef(x, 0.0f, 0.0f);
    for (j = 0; j <= abs(_hauteurs[i]); ++j) {
      GLfloat y  = (_hauteurs[i] < 0 ? -j : j) / 256.0f;
      gl4duPushMatrix();
      gl4duTranslatef(0.0f, y, 0.0f);
      // placer le scale en bout de chaine pour éviter qu'il n'agissse sur les translations précédentes
      gl4duScalef(0.02f, 0.02f, 0.02f);
      gl4duSendMatrices();
      gl4duPopMatrix();
      gl4dgDraw(_cubeId);
    }
    gl4duPopMatrix();
  }

  // Les modifications de matrices doivent être envoyées au GPU avant de dessinner
  gl4duSendMatrices();

  glUseProgram(0);
}