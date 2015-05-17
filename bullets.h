#ifndef _BULLETS_H_
#define _BULLETS_H_
//bullets.h
//Handles bullet physics/creation/

extern void fire_weapon(Game *game);
extern void bulletDraw(Bullet *b, Game *g);
extern void bulletAng(Bullet *b);
extern void updateBulletPos(Game *game, Bullet *b);
extern void bul_zomb_collision(Game *g, Bullet *b);
extern void player_loot_collision(Game *g, Loot *l);
extern void powerUp(Game *g, Loot *l);
extern void invulntime(Game *g);
extern void updateInvuln(Game *g);
extern void rftime(Game *g);
extern void updateRF(Game *g);

#endif
