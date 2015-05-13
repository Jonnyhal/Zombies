#ifndef _RENDER_H_
#define _RENDER_H_
//render.h
//Handles player/zombies textures bind

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
extern GLuint silhouetteTexture;
extern GLuint silhouette_player_Texture;

extern void render(Game *game);

#endif
