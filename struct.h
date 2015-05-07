#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <stdio.h>
#include <string.h>
#include <ctime>
#include <GL/glu.h>

extern "C" {
	#include "ppm.h"
}

#define checkImageWidth 64
#define checkImageHeight 64
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)     ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                             (c)[1]=(a)[1]-(b)[1]; \
(c)[2]=(a)[2]-(b)[2]
//macro to swap two integers
#define SWAP(x,y) (x)^=(y);(y)^=(x);(x)^=(y)
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1

typedef float Flt;
typedef float Vec[3];


//Globals--
Flt last_Position_S;
static int savex = 0;
static int savey = 0;

//-----------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double physicsCountdown=0.0;
double timeSpan=0.0;
//unsigned int upause=0;
double timeDiff(struct timespec *start, struct timespec *end) {
        return (double)(end->tv_sec - start->tv_sec ) +
                (double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
        memcpy(dest, source, sizeof(struct timespec));
}
//-----------------------------------------------------------------------------

int xres=1250, yres=900;



struct Player{
        Vec dir;
        Vec pos;
        Vec vel;
        Flt radius;
        Vec origin;
        int score;
        int currentcombo;
        int check;
        int lives;
        int invuln;
        int bulletType;
        float multi;
        int is_firing;
        float angle;
        float color[3];
        struct timespec multiTimer;
};

struct Wave {
        Ppmimage *background;
        GLuint bgTexture;
        struct Wave *next;
        struct Wave *prev;
};

struct Zone {
        Ppmimage *zbackground;
        GLuint zTexture;
        struct Zone *next;
        struct Zone *prev;
        struct Wave *wave;
};

struct Bullet {
        Vec pos;
        Vec origin;
        Vec vel;
        int type;
        float angle;
        float color[3];
        struct timespec time;
        struct Bullet *prev;
        struct Bullet *next;
};

struct Zombie {
        Vec pos;
        Vec vel;
        int nverts;
        int hitpoints;
        Flt radius;
        Vec vert[8];
        float angle;
        float rotate;
        float color[3];
        struct Zombie *prev;
        struct Zombie *next;
};

struct Loot {
        Vec pos;
        Ppmimage *lootbg;
        GLuint lootTex;
        int type;
        struct Loot *next;
        struct Loot *prev;
        struct timespec lootTimer;
};



struct Game {
        Player player1;
        Zombie *ahead;//zombies
        Bullet *bhead;
        Bullet *chead;
        Bullet *dhead;
        Zone *zhead;//zone, not zombie
        Loot *lhead;
        int gameover;
        int running;
        int lootcnt;
        int zcnt;
        int wcnt;
        int zombieSpawn;
        int nzombies;
        int nbullets;
        int startScreen;
        int current_selection;
        int old_selection;
        struct timespec bulletTimer;
        struct timespec multiTimer;
};

#endif
