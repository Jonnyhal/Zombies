#ifndef _STRUCT_H_
#define _STRUCT_H_

#include <stdio.h>
#include <string.h>
#include <ctime>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>

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
extern int xres, yres;
extern int keys[65536];
//---------------------------------------------------------------------------n
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
//unsigned int upause=0;
//-----------------------------------------------------------------------------
struct Mouse{
	Vec pos;
        Ppmimage *mousebg;
	int flag;
	float color[3];
	Mouse() {
	    pos[0] = 0;
	    pos[1] = -0;
	    pos[2] = 0.0f;
	    flag = 0;
	    color[0] = 1.0;
	    color[1] = 1.0;
	    color[2] = 1.0;
	}
};

struct Player{
        Vec dir;
        Vec pos;
        Vec vel;
        Flt radius;
        Vec origin;
        int score;
        int currentcombo;
        int check;
        int check2;
        int lives;
        int invuln;
	int tempinvuln;
        int bulletType;
        int oldbType;
	float multi;
        int is_firing;
	int tempRF;
        float angle;
        float color[3];
        struct timespec multiTimer, invulnTimer, rfTimer;
	Player() {
                VecZero(dir);
                currentcombo = 0;
                pos[0] = (Flt)(1250/2);
                pos[1] = (Flt)(900/2);
                pos[2] = 0.0f;
                origin[0] = (Flt)(1250/2);
                origin[1] = (Flt)(900/2);
                origin[2] = 0.0f;
                VecZero(vel);
                angle = 0.0;
                color[0] = 1.0;
                color[1] = 1.0;
                color[2] = 1.0;
                multi = 1.0;
                score = 0;
                check = 0;
		check2 = 0;
                radius = 20;
                invuln = 0;
                lives = 3;
                bulletType = 1;
		oldbType = 1;
		tempinvuln = 0;
		tempRF = 0;
        }
};

struct Wave {
        Ppmimage *background;
        GLuint bgTexture;
        struct Wave *next;
        struct Wave *prev;
	Wave() {
                prev = NULL;
                next = NULL;
        }
};

struct Zone {
        Ppmimage *zbackground;
        GLuint zTexture;
        struct Zone *next;
        struct Zone *prev;
        struct Wave *wave;
	Zone() {
                next = NULL;
                prev = NULL;
                wave =     NULL;
                zbackground = NULL;
        }
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
	Bullet() {
                prev = NULL;
                next = NULL;
                angle = 0.0;
                pos[0] = (Flt)(1250/2);
                pos[1] = (Flt)(900/2);
                pos[2] = 0.0f;
                VecZero(vel);
                color[0] = 1.0;
                color[1] = 1.0;
                color[2] = 1.0;
        }
};

struct Zombie {
        Vec pos;
        Vec vel;
        int nverts;
        int hitpoints;
        Flt radius;
        Vec vert[8];
	float speed;
        float angle;
        float rotate;
        float color[3];
        struct Zombie *prev;
        struct Zombie *next;
	Zombie() {
                speed = 2.0;
		prev = NULL;
                next = NULL;
                hitpoints = 1;
		angle = 0.0;
		rotate = 0.0;
        }
};

struct Loot {
        Vec pos;
        Ppmimage *lootbg;
        GLuint lootTex;
        int type;
        struct Loot *next;
        struct Loot *prev;
        struct timespec lootTimer;
	Loot() {
                prev = NULL;
                next = NULL;
                type = 0;
                clock_gettime(CLOCK_REALTIME, &lootTimer);
        }
};



struct Game {
        Player player1;
        Zombie *ahead;//zombies
        Bullet *bhead;
        Bullet *chead;
        Bullet *dhead;
        Zone *zhead;//zone, not zombie
        Mouse *cross;
	Loot *lhead;
        int gameover, start;
        int running;
        int lootcnt;
        int zcnt;
        int wcnt;
        int zombieSpawn;
        int nzombies;
        int nbullets;
        int startScreen, scoreScreen;
        int current_selection;
	int old_selection;
	int spacing, oldspacing;
        struct timespec bulletTimer;
        struct timespec multiTimer;
	Game() {
                ahead = NULL;
                bhead = NULL;
                chead = NULL;
                dhead = NULL;
                lhead = NULL;
                zhead = NULL;
                zcnt = 0;
                wcnt = 0;
                lootcnt = 0;
                nzombies = 0;
                nbullets = 0;
                startScreen = 1;
		scoreScreen = 0;
                gameover = 0;
		start = 0;
                running = 1;
                zombieSpawn = 9;
		spacing = 0;
		oldspacing = 0;
        }
};

#endif
