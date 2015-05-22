#ifndef _DELETE_H_
#define _DELETE_H_
//test

//#include "struct.h"

extern void deleteWaves(Game *g, Wave *node);
extern void deleteZone(Game *g, Zone *node);
extern void deleteLoot(Game *g, Loot *loot);
extern void deleteZombie(Game *g, Zombie *node);
extern void deleteBullet(Game *g, Bullet *node);

#endif
