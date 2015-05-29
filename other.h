#ifndef _OTHER_H_
#define _OTHER_H_
//other.h
//random function protos
//will merge into own .h files
//in the future

void init_textures(Ppmimage *image, GLuint tex);
void normalize(Vec v);
void timeCopy(struct timespec *dest, struct timespec *source);
double timeDiff(struct timespec *start, struct timespec *end);
void init_opengl(void);
void init(Game *g);
void sscreen_background(GLuint tex, float r, float g, float b, float alph);
void spawnZombies(Game *g);
void zMove(Game *g, Zombie *a);
void buildZombieFragment(Zombie *ta, Zombie *a);
void zomb_zomb_collision(Zombie *a);

#endif
