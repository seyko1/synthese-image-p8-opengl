#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <math.h>

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1024, _wH = 768;
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
  SDL_FreeSurface(s);

  
  // Bind texture 1 avec rien, juste une config similaire
  glBindTexture(GL_TEXTURE_2D, _tex[1]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  // Debind texture
  glBindTexture(GL_TEXTURE_2D, 0);

  gl4duGenMatrix(GL_FLOAT, "modelView");
  gl4duGenMatrix(GL_FLOAT, "projection");
  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, (-0.1f * _wH) / _wW, (0.1f * _wH) / _wW, 0.1f, 1000.0f); 

}

void draw(void) {
  // static int i = 0;

  GLfloat pas[] = { 1.0f / (_wW - 1.0f), 1.0f / (_wH - 1.0f) };

  // // bind fbo
  // glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  // // attacher la texture dans laquelle le fbo doit ecrire
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex[(i + 1) % 2], 0);

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // glUseProgram(_pId_conv);

  // /* Binder tex dans le tiroir à texture 0 et envoyer l'identifiant du tiroir de texture utilisé au shader */
  // /* Faire le sobel dans la texture i % 2 */
  // glActiveTexture(GL_TEXTURE0);
  // glBindTexture(GL_TEXTURE_2D, _tex[i % 2]);
  // glUniform1i(glGetUniformLocation(_pId_conv, "tex"), 0);
  // glUniform2fv(glGetUniformLocation(_pId_conv, "pas"), 1, pas);

  // gl4dgDraw(_quadId);
  // glUseProgram(0);

  // // Passer du FBO a l'écran
  // glBlitNamedFramebuffer(_pId_conv, 0, 0, 0, _wW - 1, _wH - 1, 0, 0, _wW - 1, _wH - 1, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  // ++i;
  // SDL_Delay(10);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glUseProgram(_pId_hm);


  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  glUniform1i(glGetUniformLocation(_pId_hm, "tex"), 0);
  glUniform2fv(glGetUniformLocation(_pId_hm, "pas"), 1, pas);
  gl4duBindMatrix("modelView");
  gl4duLoadIdentityf();
  gl4duLookAtf(-1.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  
  gl4dgDraw(_gridId);
  glUseProgram(0);

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
