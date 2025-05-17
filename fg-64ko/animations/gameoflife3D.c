#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <stdio.h>
#include <math.h>
#include "helpers/audioHelper.h"

#define NB_CUBES 20

/* Seuils de voisinnage pour le jeu de la vie 3D */
#define SURVIVE_MIN 1
#define SURVIVE_MAX 10
#define BIRTH_MIN 1
#define BIRTH_MAX 10

extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);

extern GLfloat _dim[];
extern GLfloat _dimBottom;
extern GLfloat _dimTop;

typedef struct {
  GLboolean enabled;
  GLint srcRGB, dstRGB;
  GLint srcAlpha, dstAlpha;
} BlendState;

static void init(void);
static void draw(void);
static void sortie(void);

static void updateGrid();
static int neighboringStates(int x, int y, int z);

static BlendState saveBlendState(void);
static void restoreBlendState(const BlendState * state);

static GLuint _cubeId = 0;
static GLuint _sphereId = 0;
static GLuint _pId = 0;

static GLboolean grid[NB_CUBES][NB_CUBES][NB_CUBES];
static GLfloat colorAlive[]       = { 0.5f, 0.7f, 0.4f, 1.0f };
static const GLfloat colorDead[]  = { 1.0f, 1.0f, 1.0f, 0.4f };

static GLint colorUniformLocation;
static GLint isAliveUniformLocation;

static GLboolean canUpdateGrid = GL_FALSE;

struct gstatic_t {
  GLuint buffer;
};

struct geom_t {
  GLuint vao;
  void * geom;
};

typedef struct geom_t geom_t;
typedef struct gstatic_t gstatic_t;

static geom_t * _cubeEdgeId = NULL;

static geom_t * genCubeEdges() {
  static GLfloat cube_data[] = {
    /* front */
    -1.0f, -1.0f, 1.0f,
     1.0f, -1.0f, 1.0f,
     1.0f,  1.0f, 1.0f,
    -1.0f,  1.0f, 1.0f,
    /* back */
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    /* right */
    1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    /* left */
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    /* top */
    -1.0f, 1.0f,  1.0f,
     1.0f, 1.0f,  1.0f,
     1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    /* bottom */
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
  };
  geom_t * geom = malloc(sizeof *geom);
  assert(geom);
  gstatic_t * q = malloc(sizeof *q);
  assert(q);
  geom->geom = q;

  glGenVertexArrays(1, &geom->vao);
  glBindVertexArray(geom->vao);

  glEnableVertexAttribArray(0); 
  
  glGenBuffers(1, &(q->buffer));
  glBindBuffer(GL_ARRAY_BUFFER, q->buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof cube_data, cube_data, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return geom;
}

void drawCubeEdges(GLint vao) {
  glBindVertexArray(vao);
  glDrawArrays(GL_LINE_LOOP, 0, 4);
  glDrawArrays(GL_LINE_LOOP, 4, 4);
  glDrawArrays(GL_LINE_LOOP, 8, 4);
  glDrawArrays(GL_LINE_LOOP, 12, 4);
  glDrawArrays(GL_LINE_LOOP, 16, 4);
  glDrawArrays(GL_LINE_LOOP, 20, 4);
  glBindVertexArray(0);
}

void gameoflife3D(int state) {
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
      int moyenne = value / (length / 4);

      float t = gl4dGetElapsedTime() / 1000;
      if (t < 27. && moyenne > 2500) {
        canUpdateGrid = GL_TRUE;
      } else if (moyenne > 3000) {
        canUpdateGrid = GL_TRUE;
      } else {
        canUpdateGrid = GL_FALSE;
      }
      return;
    default:
      draw();
      return;
  }
}

void init(void) {
  initNoiseTextures();

  const float middle = NB_CUBES / 2;
  const int margin = 2;
  const int min = middle - margin;
  const int max = middle + margin;

  for (int i = 0; i < NB_CUBES; ++i) {
    for (int j = 0; j < NB_CUBES; ++j) {
      for (int k = 0; k < NB_CUBES; ++k) {
        if (
          i >= min && i <= max &&
          j >= min && j <= max &&
          k >= min && k <= max
        ) {
          grid[i][j][k] = GL_TRUE;
        } else {
          grid[i][j][k] = GL_FALSE;
        }
      }
    }
  }
  _cubeId = gl4dgGenCubef();
  _sphereId = gl4dgGenSpheref(3, 3);
  _cubeEdgeId = genCubeEdges();

  _pId = gl4duCreateProgram("<vs>shaders/gameoflife3D.vs", "<fs>shaders/gameoflife3D.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "projection");

  colorUniformLocation = glGetUniformLocation(_pId, "color");
  isAliveUniformLocation = glGetUniformLocation(_pId, "isAlive");
}

static double get_dt(void) {
  static double t0 = 0.0f;
  // Récupérer le temps écoulé en ms entre le démarrage du programme et la frame en cours
  double t = gl4dGetElapsedTime();
  // Calculer le temps écoulé depuis la frame précédente en seconde
  double dt = (t - t0) / 100.0f;
  t0 = t;
  return dt;
}

void drawCube(int i, int j, int k) {
  static const float scale = 0.04f;

  // Ramener à l'échelle [-1;1]
  static const float normalizedFactor = 2.0f / (NB_CUBES - 1.0f);
  float x = i * normalizedFactor - 1.0f;
  float y = j * normalizedFactor - 1.0f;
  float z = k * normalizedFactor - 1.0f;

  GLboolean isAlive = grid[i][j][k] == GL_TRUE;
  const GLfloat * color = isAlive ? colorAlive : colorDead;

  /*  Enregistrer le polygon mode */
  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);
  /* Définir le polygon mode pour le cube actuel */
  GLenum mode = isAlive ? GL_FILL : GL_LINE;

  gl4duPushMatrix();
  gl4duTranslatef(x, y, z);
  gl4duScalef(scale, scale, scale);
  gl4duSendMatrices();
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glUniform1i(isAliveUniformLocation, isAlive);
  glUniform4fv(colorUniformLocation, 1, color);
  
  if (isAlive) {
    gl4dgDraw(_cubeId);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform4fv(colorUniformLocation, 1, colorDead);
    glLineWidth(2.0);
    drawCubeEdges(_cubeEdgeId->vao);
    glLineWidth(1.0);
  } else {
    drawCubeEdges(_cubeEdgeId->vao);
  }
  gl4duPopMatrix();

  // Rétablir l'état initial du polygon mode
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
}

void draw(void) {
  /* Activer l'alpha pour le mode fil de fer */
  BlendState blendState = saveBlendState();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  /* Sauvegarde du depth test mode */
  GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  float dt = get_dt();
  static GLfloat rot = 0.0f;
  
  // Modifier la hue value
  colorAlive[0] += dt * 0.2f;
  if (colorAlive[0] > 1.0f) colorAlive[0] -= 1.0f;
  
  glUseProgram(_pId);

  gl4duBindMatrix("projection");
  gl4duLoadIdentityf();
  
  gl4duFrustumf(-1.0f, 1.0f, _dimBottom, _dimTop, 1.0f, 100.0f);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();

  float radius = 3.0f;
  float camX = radius * sinf(rot);
  float camZ = radius * cosf(rot);
  float camY = 1.2f + 0.3f * sinf(rot * 0.5f);

  gl4duLookAtf(camX, camY, camZ,
               0.0f, 0.0f, 0.0f,
               0.0f, 1.0f, 0.0f);

  gl4duBindMatrix("model");
  gl4duLoadIdentityf();

  glUniform1i(glGetUniformLocation(_pId, "useNoise"), 1);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  gl4duScalef(10.0f, 10.0f, 10.0f);
  gl4duSendMatrices();
  useNoiseTextures(_pId, 0);
  gl4dgDraw(_sphereId);
  unuseNoiseTextures(0);
  
  gl4duLoadIdentityf();
  glUniform1i(glGetUniformLocation(_pId, "useNoise"), 0);

  for (int i = 0; i < NB_CUBES; ++i)
    for (int j = 0; j < NB_CUBES; ++j)
      for (int k = 0; k < NB_CUBES; ++k)
        drawCube(i, j, k);

  glUseProgram(0);

  if (canUpdateGrid) updateGrid();

  /* Restaurer depth test et blend */
  if (!wasDepthEnabled) {
    glDisable(GL_DEPTH_TEST);
  }
  restoreBlendState(&blendState);

  rot += 0.02f * M_PI * get_dt();
}

void updateGrid() {
  GLboolean nextGen[NB_CUBES][NB_CUBES][NB_CUBES];

  for (int i = 0; i < NB_CUBES; ++i) {
    for (int j = 0; j < NB_CUBES; ++j) {
      for (int k = 0; k < NB_CUBES; ++k) {
        int n = neighboringStates(i, j, k);

        if (grid[i][j][k] == GL_TRUE) {
          if (n >= SURVIVE_MIN && n <= SURVIVE_MAX) {
            nextGen[i][j][k] = GL_TRUE;
          } else {
            nextGen[i][j][k] = GL_FALSE;
          }
        }
        else {
          if (n >= BIRTH_MIN && n <= BIRTH_MAX) {
            nextGen[i][j][k] = GL_TRUE;
          } else {
            nextGen[i][j][k] = GL_FALSE;
          }
        }
      }
    }
  }
  memcpy(grid, nextGen, sizeof(grid));
}

int neighboringStates(int x, int y, int z) {
  int sum = 0;
  int xIndex = 0, yIndex = 0, zIndex = 0;

  for (int i = -1; i < 2; ++i) {
    for (int j = -1; j < 2; ++j) {
      for (int k = -1; k < 2; ++k) {
        /* Sauter à l'index de départ ou de fin en cas de dépassement */
        /* (-1 + 10) % 10 = 9 */
        xIndex = (x + i + NB_CUBES) % NB_CUBES;
        yIndex = (y + j + NB_CUBES) % NB_CUBES;
        zIndex = (z + k + NB_CUBES) % NB_CUBES;
        sum += grid[xIndex][yIndex][zIndex];
      }
    }
  }

  sum -= grid[x][y][z];
  return sum;
}

/* Sauvegarde l'état du blend et la façon dont il est fait avant d'y toucher dans le contexte openGL */
BlendState saveBlendState(void) {
  BlendState state;
  state.enabled = glIsEnabled(GL_BLEND);
  glGetIntegerv(GL_BLEND_SRC_RGB, &state.srcRGB);
  glGetIntegerv(GL_BLEND_DST_RGB, &state.dstRGB);
  glGetIntegerv(GL_BLEND_SRC_ALPHA, &state.srcAlpha);
  glGetIntegerv(GL_BLEND_DST_ALPHA, &state.dstAlpha);
  return state;
}

/* Restaure le blend dans le contexte openGL */
void restoreBlendState(const BlendState *state) {
  if (state->enabled)
    glEnable(GL_BLEND);
  else
    glDisable(GL_BLEND);

  glBlendFuncSeparate(state->srcRGB, state->dstRGB, state->srcAlpha, state->dstAlpha);
}

void sortie(void) {
  if (_cubeEdgeId) {
    gstatic_t * q = (gstatic_t *)_cubeEdgeId->geom;
    glDeleteVertexArrays(1, &_cubeEdgeId->vao);
    glDeleteBuffers(1, &q->buffer);
    free(q);
    free(_cubeEdgeId);
    _cubeEdgeId = NULL;
  }

  freeNoiseTextures();
}
