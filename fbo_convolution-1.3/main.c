#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <SDL_image.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1280, _wH = 1024;
static GLuint _imageW = 100, _imageH = 75;
static GLuint _quadId = 0;
static GLuint _gridId = 0;
static GLuint _pId_conv = 0, _pId_hm = 0;
static GLuint _fbo = 0;
static GLuint _tex[2] = { 0, 0 };

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
  SDL_Surface * s;

  glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

  _quadId   = gl4dgGenQuadf();
  _pId_conv = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/blur.fs", NULL);
  _pId_hm   = gl4duCreateProgram("<vs>shaders/hm.vs", "<fs>shaders/hm.fs", NULL);

  glGenFramebuffers(1, &_fbo);

  /* Gestion de la texture pour le fbo */

  glGenTextures(2, _tex),

  // Bind texture 0
  glBindTexture(GL_TEXTURE_2D, _tex[0]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  s = IMG_Load("images/gl4d.png");
  assert(s);
  
  _gridId = gl4dgGenGrid2df(s->w, s->h);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  
  // Bind texture 1 avec les mêmes paramètres
  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  
  SDL_FreeSurface(s);

  // Debind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, (-0.1f * _wH) / _wW, (0.1f * _wH) / _wW, 0.1f, 1000.0f); 
}

void draw(void) {
  static int i = 0, inc = 1;

  GLfloat pas[] = { 1.0f / (_imageW - 1.0f), 1.0f / (_imageH - 1.0f) };

  gl4dfBlur(_tex[0], _tex[1], 3, i, 0, GL_FALSE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(_pId_hm);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  glUniform1i(glGetUniformLocation(_pId_hm, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId_hm, "pas"), 1, pas);
  
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duLookAtf(-1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  
  gl4dgDraw(_gridId);

  if ((inc > 0 && i > 32) || (inc < 0 && i < 0)) {
    inc = -inc;
  }
  i += inc;

  SDL_Delay(100);
}

void sortie(void) {
  if (_fbo) {
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if (_tex[0]) {
    glDeleteTextures(2, _tex);
    _tex[0] = 0;
  }
  gl4duClean(GL4DU_ALL);
}
