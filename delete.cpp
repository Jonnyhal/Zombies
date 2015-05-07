#include <stdlib.h>

extern "C++"{
	#include "struct.h"
}

extern void deleteWaves(Game *g, Wave *node);
extern void deleteZone(Game *g, Zone *node);
extern void deleteLoot(Game *g, Loot *loot);
extern void deleteZombie(Game *g, Zombie *node);
extern void deleteBullet(Game *g, Bullet *node);

void deleteLoot(Game *g, Loot *loot)
{
    if (loot){
        //remove a node from linked list
        if (loot->prev == NULL) {
            if (loot->next == NULL) {
                g->lhead = NULL;
            } else {
                loot->next->prev = NULL;
                g->lhead = loot->next;
            }
        } else {
            if (loot->next == NULL) {
                loot->prev->next = NULL;
            } else {
                loot->prev->next = loot->next;
                loot->next->prev = loot->prev;
            }
        }
        delete loot;
        loot = NULL;
        g->lootcnt--;
    }
}

void deleteBullet(Game *g, Bullet *node)
{
        //remove a node from linked list
        if (node->type == 1) {
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->bhead = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->bhead = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        } else if (node->type == 2) {
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->chead = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->chead = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        } else if (node->type == 3) {
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->dhead = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->dhead = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        }
}

void deleteZombie(Game *g, Zombie *node)
{
        //remove a node from linked list
        if (g){}
        if (node){
                //remove a node from linked list
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->ahead = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->ahead = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        }
}

void deleteZone(Game *g, Zone *node)
{
        //remove a node from linked list
        deleteWaves(g, node->wave);
        if (node){
                //remove a node from linked list
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->zhead = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->zhead = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        }
}

void deleteWaves(Game *g, Wave *node)
{
        //remove a node from linked list
        if (g){}
        if (node){
                //remove a node from linked list
                if (node->prev == NULL) {
                        if (node->next == NULL) {
                                g->zhead->wave = NULL;
                        } else {
                                node->next->prev = NULL;
                                g->zhead->wave = node->next;
                        }
                } else {
                        if (node->next == NULL) {
                                node->prev->next = NULL;
                        } else {
                                node->prev->next = node->next;
                                node->next->prev = node->prev;
                        }
                }
                delete node;
                node = NULL;
        }
}

