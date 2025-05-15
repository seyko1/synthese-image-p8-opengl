#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>
#include "helpers/audioHelper.h"

#define HEIGHTS_SIZE 16

extern GLfloat _dim[];

static GLfloat _dimBottom;
static GLfloat _dimTop;

static void init(void);
static void draw(void);
static void sortie(void);

static GLuint _gridId = 0;
static GLuint _pId = 0;
static GLuint _tex = 0;

static GLuint _hauteurs[HEIGHTS_SIZE] = { 0 };
static GLbyte _gridW = 64, _gridH = 64;

/* Remplace le 1er élément du tableau et décale les autres */
void shift(short v) {
  for (int i = HEIGHTS_SIZE - 1; i > 0; i--) {
    _hauteurs[i] = _hauteurs[i - 1];
  }

  _hauteurs[0] = v;
}

static short moyenne = 0;
void ondulations(int state) {
  switch(state) {
    case GL4DH_INIT:
      init();
      return;
    case GL4DH_FREE:
      sortie();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      int length = ahGetAudioStreamLength();
      short * stream = (short*) ahGetAudioStream(); /* échantillon de 4096 valeurs */
      int value = 0;
      /* Avancer de 2 pour ne prendre qu'une des 2 valeurs du stéréo */
      for (int i = 0; i < length / 4; ++i) {
        // somme des 1024 valeurs d'oreille gauche
        value += fabs(stream[2 * i]);
      }
      moyenne = value / (length / 4);
      shift(moyenne);
      return;
    default:
      draw();
      return;
  }
}

void init(void) {
  _dimBottom = (-0.1f * _dim[1]) / _dim[0];
  _dimTop = (0.1f * _dim[1]) / _dim[0];

  _pId = gl4duCreateProgram("<vs>shaders/ondulations.vs", "<fs>shaders/ondulations.fs", NULL);
  _gridId = gl4dgGenGrid2df(_gridW, _gridH);

  glGenTextures(1, &_tex),
  glBindTexture(GL_TEXTURE_2D, _tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "projection");
}

static double get_dt(void) {
  static double t0 = 0.0f;
  double t = gl4dGetElapsedTime();
  double dt = (t - t0) / 100.0f;
  t0 = t;
  return dt;
}

void draw(void) {
  GLfloat pas[] = { 1.0f / (_gridW - 1.0f), 1.0f / (_gridH - 1.0f) };

  static GLfloat rot = 0.0f;
  
  glClearColor(0.0f, 0.7f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(_pId);
  
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, _dimBottom + 0.02f, _dimTop - 0.02f, 0.1f, 1000.0f); 
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex);
  // Envoyer les hauteurs dans une texture de dimension 1 * HEIGHTS_SIZE
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, 1, HEIGHTS_SIZE, 0, GL_RED, GL_SHORT, _hauteurs);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId, "pas"), 1, pas);
  glUniform1f(glGetUniformLocation(_pId, "elapsed"), (GLfloat)gl4dGetElapsedTime() / 1000);
  
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();

  float camX = 3.0f * sinf(rot);
  float camZ = 2.0f * cosf(rot);
  float camY = 1.2f + 0.3f * sinf(rot * 0.5f);
  
  gl4duLookAtf(camX, camY, camZ,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);
    
  gl4duScalef(2.0f, 1.0f, 2.0f);
  gl4duSendMatrices();
  
  gl4dgDraw(_gridId);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);

  rot += 0.02f * M_PI * get_dt();
}

void sortie(void) {
  if (_tex) {
    glDeleteTextures(1, &_tex);
    _tex = 0;
  }
}
