#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>
#include "audioHelper.h"

#define NBE 32

static void init(void);
static void draw(void);
static void sortie(void);

static GLuint _wW = 1280, _wH = 1024;
static GLuint _gridId = 0;
static GLuint _pId = 0;
static GLuint _tex = 0;

static GLuint _hauteurs[NBE] = { 0 };
static GLbyte _gridW = 64, _gridH = 64;

/* Remplace le 1er élément du tableau et décale les autres */
void shift(short v) {
  for (int i = NBE - 1; i > 0; i--) {
    _hauteurs[i] = _hauteurs[i - 1];
  }

  _hauteurs[0] = v;
}

static short moyenne = 0;
void grid(int state) {
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
  _pId = gl4duCreateProgram("<vs>shaders/grid.vs", "<fs>shaders/grid.fs", NULL);
  _gridId = gl4dgGenGrid2df(_gridW, _gridH);

  glGenTextures(1, &_tex),
  glBindTexture(GL_TEXTURE_2D, _tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
 
  gl4duFrustumf(-0.1f, 0.1f, (-0.1f * _wH) / _wW, (0.1f * _wH) / _wW, 0.1f, 1000.0f); 
}

void draw(void) {
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  glUseProgram(_pId);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex);
  // Envoyer les hauteurs dans une texture de dimension 1 * NBE
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, 1, NBE, 0, GL_RED, GL_SHORT, _hauteurs);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duLookAtf(-1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  
  gl4dgDraw(_gridId);
  
  glBindTexture(GL_TEXTURE_2D, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glUseProgram(0);
}

void sortie(void) {
  if (_tex) {
    glDeleteTextures(1, &_tex);
    _tex = 0;
  }
}
