#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 640, _wH = 480;
static GLuint _sphereId = 0;
static GLuint _pId = 0;
static GLuint _fbo = 0;
static GLuint _tex = 0;

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - Convolution FBO", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

  _sphereId = gl4dgGenSpheref(7, 7);
  _pId = gl4duCreateProgram("<vs>shaders/conv.vs", "<fs>shaders/conv.fs", NULL);

  /* Gestion de la texture pour le fbo */

  glGenTextures(1, &_tex),

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, _tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // Debind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Gestion du FBO */
  
  glGenFramebuffers(1, &_fbo);

  // Bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  // Attacher la texture au fbo
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);
  // Debind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw(void) {
  // bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);
  glUniform1f(glGetUniformLocation(_pId, "weight"), 1.1);
  gl4dgDraw(_sphereId);
  glUseProgram(0);

  // debind fbo -> rebind l ecran en écriture pour indiquer que l'écran est la source
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  glBlitFramebuffer(
    0, 0, _wW - 1, _wH -1,
    0, 0, _wW - 1, _wH -1,
    GL_COLOR_BUFFER_BIT, GL_NEAREST
  );

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void sortie(void) {
  if (_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if (_tex) {
    glDeleteTextures(1, &_tex);
    _tex = 0;
  }
  gl4duClean(GL4DU_ALL);
}
