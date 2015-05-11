#ifndef _BULLETS_H_
#define _BULLETS_H_
//bullets.h
//Handles bullet physics/creation/

extern void fire_weapon(Game *game);
extern void bulletDraw(Bullet *b);
extern void bulletAng(Bullet *b);
extern void updateBulletPos(Game *game, Bullet *b);
extern void bul_zomb_collision(Game *g, Bullet *b);

#endif
