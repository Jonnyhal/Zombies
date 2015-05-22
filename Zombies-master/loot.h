#ifndef _LOOT_H_
#define _LOOT_H_
//loot.h
//functions for loot and multi

extern void multitime(Game *g);
extern void updateMulti(Game *g);
extern void lootDraw(GLuint tex, Loot *l, float r, float g, float b, float alph);
extern void lootDrop(Game *g, Zombie *a);
extern void lootTime(Loot *l);

#endif
