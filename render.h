#ifndef _RENDER_H_
#define _RENDER_H_
//render.h
//Handles player/zombies textures bind

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
extern GLuint silhouetteTexture;
extern GLuint silhouette_player_Texture;
extern GLuint LFsilhouette_player_Texture;
extern GLuint RFsilhouette_player_Texture;
extern GLuint silhouette_player_Texture_2;
extern GLuint silhouette_player_Texture_3;
extern GLuint silhouette_mouse_Texture;

extern void render(Game *game);

#endif
