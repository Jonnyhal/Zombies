//cs335 Spring 2015 GAME PROJECT
//zombiesarc.cpp
//Authors: Brandon Ware, Christy, Juban, Jonathan
//
//
//
//Original Program:
//  program: asteroid.cpp
//  author:  Gordon Griesel
//  date:    2014
//  mod spring 2015: added constructors
//
//This program is a game starting point for 335
//
// Requirements:                      Add a finish date if you complete any of these ~bware
// ----------------------------------------------------------------------------------------
// Start screen with menu selection   [X]
// Keyboard + mouse = move + aim      [X] finished in first gitpush
// Strafe/ Diagonal movement          [X] finished in first gitpush
// At least four weapon types         []
// Zombie spawn/movement algorithm    [X]
// Score counter ingame               [X] -- 
// Score record keeping               [] -- Should we keep the file online or local?
// collision detection zombie->player [] -- insta-kill when touched? or health value?
// Zombie Textures                    [] --
// Player Textures                    [] -- how many textures to create walk cycle?
// Blood and Death textures for P+Z   [] -- different death textures, or only one?
// BACKGROUND                         [/] -- single texture or multi? possibly 3d?
// Grid to store "rooms"              [] -- how many "rooms"? Room design? difficulty?
// SOUND                              [] -- Yeah... there is a lot of stuff here...
// Zombie/Player health values        []
// Pause Screen                       [/] -- I think this would be a good idea to add ~bware
//
// Possible Additions
// ----------------------
// Difficulty setting
// Destructables
// Ammo capacity/refills
// 
//
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <GL/gl.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
extern "C" {
#include "fonts.h"
#include "ppm.h"
}

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt	Matrix[4][4];

//macros -- move these to a separate file? -bware
#define checkImageWidth 64
#define checkImageHeight 64
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
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

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;
GC gc;

//globals
Flt last_Position_S;
static int savex = 0;
static int savey = 0;
//
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

//images/textures
//-----------------------------------------------------------------------------
Ppmimage *background0 = NULL;
Ppmimage *gameover0 = NULL;
Ppmimage *player1 = NULL;
Ppmimage *zombie0 = NULL;

//
GLuint bgTexture0;
GLuint gameoverTex;
GLuint player1Tex;
GLuint zombieTex;

struct Player {
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
	Player() {
		VecZero(dir);
		currentcombo = 0;
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		origin[0] = (Flt)(xres/2);
		origin[1] = (Flt)(yres/2);
		origin[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
		multi = 1.0;
		score = 0;
		check = 0;
		radius = 20;
		invuln = 0;
		lives = 3;
		bulletType = 1;
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
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
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
	float angle;
	float rotate;
	float color[3];
	struct Zombie *prev;
	struct Zombie *next;
	Zombie() {
		prev = NULL;
		next = NULL;
		hitpoints = 1;
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
		gameover = 0;
		running = 1;
		zombieSpawn = 5;
	}
};


int keys[65536];

//function prototypes
void zomb_zomb_collision(Zombie *a);
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e);
void init(Game *g);
void init_textures(Ppmimage *image, GLuint tex);
void spawnZombies(Game *g);
void init_sounds(void);
void physics(Game *game);
void fire_weapon(Game *game);
void bulletDraw(Bullet *b);
void bulletAng(Bullet *b); 
void updateBulletPos(Game *game, Bullet *b); 
void bul_zomb_collision(Game *g, Bullet *b);
void render(Game *game);
void bresenham_Ang(Game *g);
void render_StartScreen(Game *game);
void sscreen_background(GLuint tex, float r, float g, float b, float alph);
void deleteZone(Game *g, Zone *node);
void deleteWaves(Game *g, Wave *node);
void deleteLoot(Game *g, Loot *loot);
void renderscoreScreen(Game *g);
void rendergameoverScreen(Game *g);
void multitime(Game *g);
void updateMulti(Game *g);
void lootDraw(GLuint tex, Loot *l, float r, float g, float b, float alph);
void lootDrop(Game *g, Zombie *a);
void lootTime(Loot *l);
int fib(int n);
void zMove(Game *g);
void screen1(Game *game);
void screen2(Game *game);

int main(void)
{
	logOpen();
	initXWindows();
	init_opengl();
	Game game;
	game.current_selection = 1;
	int donesscreen = 0;
	//sscreen_background(&game);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	int done=0;
	while (game.running) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, &game);
			done = check_keys(&e);
			if (done)
				game.running = 0;
			if (game.startScreen)
				screen1(&game);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics(&game);
			physicsCountdown -= physicsRate;
			if (game.gameover) {
				screen2(&game); 
				break;
			}
		}
		//if (game.gameover)
		//	screen2(&game); 
		render(&game);
		glXSwapBuffers(dpy, win);
		
	}
	cleanupXWindows();
	cleanup_fonts();
	logClose();
	return 0;
}

void screen1(Game *game) //start screen
{
	int donesscreen = 0;
	while (game->startScreen) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, game);
			if((donesscreen = check_keys(&e))) {//NOT comparing, setting and checking value for 0/1
				game->startScreen = 0;
				game->running = 0;
			}
		}
		render_StartScreen(game);
		glXSwapBuffers(dpy, win);
	}
	//cleanupXWindows();
	//cleanup_fonts();
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//init_opengl();
	//game->zombieSpawn = 60;
	//init(game);
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	game->player1.is_firing = 0;
	game->startScreen = 0;
	//we should make a player initialization function
}

void screen2(Game *game) //game over screen
{
	//std::cout<<"made it here\n";
	int donesscreen = 0;
	while (game->gameover) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, game);
			if((donesscreen = check_keys(&e))) {//NOT comparing, setting and checking value for 0/1
				game->running = 0;
				game->gameover = 0;
			}
		}
		rendergameoverScreen(game);
		glXSwapBuffers(dpy, win);
	}
	game->gameover = 0;
}

void cleanupXWindows(void)
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "DIE ZOMBIE DIE");
}

void setup_screen_res(const int w, const int h)
{
	xres = w;
	yres = h;
}

void initXWindows(void)
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
	XSetWindowAttributes swa;
	setup_screen_res(xres, yres);
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask | ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask;
	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	gc = XCreateGC(dpy, win, 0, NULL);
	glXMakeCurrent(dpy, win, glc);
	unsigned long cref = 0L;
	cref += 200;
	cref <<= 8;
	cref += 200;
	cref <<= 8;
	cref += 200;
	XSetForeground(dpy, gc, cref);
	memset(keys, 0, 65536);
}

void reshape_window(int width, int height)
{
	//window has been resized.
	setup_screen_res(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	set_title();
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

	//Load image files
	background0 = ppm6GetImage("./images/ssbg.ppm");
	gameover0   = ppm6GetImage("./images/mygameover.ppm");
	player1     = ppm6GetImage("./images/soldier.ppm");
	zombie0     = ppm6GetImage("./images/zombie.ppm");
	//Generate Textures
	glGenTextures(1, &bgTexture0);
	init_textures(background0, bgTexture0);
	glGenTextures(1, &gameoverTex);
	init_textures(gameover0, gameoverTex);
	glGenTextures(1, &player1Tex);
	init_textures(player1, player1Tex);
	glGenTextures(1, &zombieTex);
	init_textures(zombie0, zombieTex);

}

void init_textures(Ppmimage *image, GLuint tex)
{
	//glGenTextures(1, &tex);
	glClearColor(1.0, 0.0, 0.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,image->width, image->height,
			0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
}

void check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != xres || xce.height != yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}

void init(Game *g) {
	// before calling, show intro text
	// ex Round 1 Wave 1 
	//        START!
	//initialize level+spawn zombs
	//check before if zomb==0, init()
	std::cout<<"zcnt: " << g->zcnt;
	std::cout<<" wcnt: " << g->wcnt;

	if (g->wcnt > 3) {
		Zone *z = new Zone;
		z->wave = new Wave;
		deleteZone(g,g->zhead);
		g->zhead = z;
		g->zcnt++;
		g->wcnt = 1;
		g->zhead->zbackground = ppm6GetImage("./images/tex3check.ppm");
		glClearColor(1.0, 0.0, 0.0, 1.0);
		//Do this to allow fonts
		glEnable(GL_TEXTURE_2D);
		initialize_fonts();

		glGenTextures(1, &g->zhead->zTexture);
		glBindTexture(GL_TEXTURE_2D, g->zhead->zTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3,g->zhead->zbackground->width, 
				g->zhead->zbackground->height,0, GL_RGB, GL_UNSIGNED_BYTE, 
				g->zhead->zbackground->data);

	} else if (g->zhead == NULL) {
		Zone *z = new Zone;
		z->wave = new Wave;
		g->zhead = z;
		g->zcnt = 1;
		g->wcnt = 1;
		g->zhead->zbackground = ppm6GetImage("./images/tex2.ppm");
		glClearColor(1.0, 0.0, 0.0, 1.0);
		//Do this to allow fonts
		glEnable(GL_TEXTURE_2D);
		initialize_fonts();

		glGenTextures(1, &g->zhead->zTexture);
		glBindTexture(GL_TEXTURE_2D, g->zhead->zTexture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3,g->zhead->zbackground->width, 
				g->zhead->zbackground->height,0, GL_RGB, GL_UNSIGNED_BYTE, 
				g->zhead->zbackground->data);
	} else {
		Wave *w = new Wave;
		w->next = g->zhead->wave;
		g->zhead->wave->prev = w;
		g->zhead->wave = w;
		g->wcnt++;
	}
	spawnZombies(g);
}

void spawnZombies(Game *g) 
{
	std::cout<<"\nwcnt in spawnZombies: " << g->wcnt << "\n";
	std::cout<<"zcnt in spawnZombies: " << g->zcnt << "\n";
	//build x zombies... where x = zombiespawner
	//for (int j=0; j<g->zombieSpawner; j++) {
	for (int j=0; j<g->zombieSpawn; j++) {
		Zombie *a = new Zombie;
		a->nverts = 4;
		a->radius = 20.0;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
			a->vert[i][0] = sin(angle) * (r2 + a->radius);
			a->vert[i][1] = cos(angle) * (r2 + a->radius);
			angle += inc;
		}
		a->angle = 0.0;
		a->hitpoints = g->zcnt - 1;
		//std::cout << "asteroid" << std::endl;
		//left part of screen 3/4 down from top
		if((j%5)==0){
			a->pos[0] = (Flt)(0);
			a->pos[1] = (Flt)(yres*0.65);
			a->pos[2] = 0.0f;
			a->color[0] = 0.5;
			a->color[1] = 2.5;
			a->color[2] = 1.0;
			a->vel[0] = (Flt)(rnd()*2.0);
			a->vel[1] = (Flt)(0);
		}
		//left part of screen 1/4 down from the top
		if((j%5)==1){
			a->pos[0] = (Flt)(0);
			a->pos[1] = (Flt)(yres*0.25);
			a->pos[2] = 0.0f;
			a->color[0] = 0.5;
			a->color[1] = 2.5;
			a->color[2] = 1.0;
			a->vel[0] = (Flt)(rnd()*2.0);
			a->vel[1] = (Flt)(0);
		}
		//bottom
		if((j%5)==2){
			a->pos[0] = (Flt)(xres*0.25);
			a->pos[1] = (Flt)(0);
			a->pos[2] = 0.0f;
			a->color[0] = 0.5;
			a->color[1] = 2.5;
			a->color[2] = 1.0;
			a->vel[0] = (Flt)(0);
			a->vel[1] = 10;
		}
		//right middle
		if((j%5)==3){
			a->pos[0] = (Flt)(xres);
			a->pos[1] = (Flt)(yres*0.5);
			a->pos[2] = 0.0f;
			a->color[0] = 0.5;
			a->color[1] = 2.5;
			a->color[2] = 1.0;
			a->vel[0] = (Flt)(rnd()*(-2.0));
			a->vel[1] = (Flt)(rnd()*(0));
		}
		//top
		if((j%5)==4){
			a->pos[0] = (Flt)(xres*0.65);
			a->pos[1] = (Flt)(yres);
			a->pos[2] = 0.0f;
			a->color[0] = 0.5;
			a->color[1] = 2.5;
			a->color[2] = 1.0;
			a->vel[0] = (Flt)(0);
			a->vel[1] = (Flt)(rnd()*(-2.0));
		}	
		//add to front of linked list
		a->next = g->ahead;
		if (g->ahead != NULL)
			g->ahead->prev = a;
		g->ahead = a;
		g->nzombies++;
	}
	clock_gettime(CLOCK_REALTIME, &g->bulletTimer);
	clock_gettime(CLOCK_REALTIME, &g->multiTimer);
	clock_gettime(CLOCK_REALTIME, &g->player1.multiTimer);
	g->player1.radius = 10;

}
//======Zombie movement function=======
void zMove(Game *g, Zombie *a)
{
	Flt d0, d1, dist;
	float x0, y0, x1, y1;
	//zombie to player movement
	d0 = g->player1.pos[0] - a->pos[0];
	d1 = g->player1.pos[1] - a->pos[1];
	dist = sqrt(d0*d0 + d1*d1);
	if (dist < 700) {
		a->vel[0] = d0/dist * 5.0;
		a->vel[1] = d1/dist * 5.0;
	}
	if (g->player1.origin[0] != g->player1.pos[0] || g->player1.origin[1] != g->player1.pos[1]) {
		a->vel[0] = d0/dist * 5.0;
		a->vel[1] = d1/dist * 5.0;
	}
	//rotate zombie to chase player
	x0 = g->player1.pos[0], y0 = g->player1.pos[1];
	x1 = a->pos[0], y1 = a->pos[1];
	float tmpx = x1-x0;
	float tmpy = y1-y0;
	Flt hypot = sqrt(tmpx*tmpx + tmpy*tmpy);
	Flt trig = sqrt(tmpy/hypot * tmpy/hypot); 
	Flt angle = ((asin(trig)*100)/1.74444444);

	if(tmpx > 0 && tmpy > 0) {

		angle = 270 + angle;
		a->angle = angle;
	}
	if(tmpx > 0 && tmpy < 0) {
		angle = 270 - angle;
		a->angle = angle;
	}
	if(tmpx < 0 && tmpy < 0) {
		angle = 90 + angle;
		a->angle = angle;
	}
	if(tmpx < 0 && tmpy > 0) {
		angle = 90 - angle;
		a->angle = angle;
	}
}
//====================================
void normalize(Vec v) 
{
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f) {
		v[0] = 1.0;
		v[1] = 0.0;
		return;
	}
	len = 1.0f / sqrt(len);
	v[0] *= len;
	v[1] *= len;
}

//          player1 x , player1 y,  mouse x, mouse y
void bresenham_Ang(Game *g) 
{
	// I used a TON of divides... maybe look to optimize? ~bware
	// fixed... mostly  ~bware
	//Calculate where to angle and shoot based on pointer
	//int x, y, xDiff, yDiff, err;
	float x0, y0, x1, y1;
	x0 = g->player1.pos[0], y0 = g->player1.pos[1];
	x1 = savex, y1 = savey;
	y1 = yres - y1;
	float tmpx = x1-x0;
	float tmpy = y1-y0;
	Flt hypot = sqrt(tmpx*tmpx + tmpy*tmpy);
	Flt trig = sqrt(tmpy/hypot * tmpy/hypot); 
	Flt angle = ((asin(trig)*100)/1.74444444);

	if(tmpx > 0 && tmpy > 0) {

		angle = 270 + angle;
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
	}
	if(tmpx > 0 && tmpy < 0) {
		angle = 270 - angle;
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;
	}
	if(tmpx < 0 && tmpy < 0) {
		angle = 90 + angle;
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;
	}
	if(tmpx < 0 && tmpy > 0) {
		angle = 90 - angle;
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
	}

}
/*           bullet x, y,     origin x, y */
void bulletAng(Bullet *b) 
{
	float x0, y0, x1, y1;
	x0 = b->pos[0], y0 = b->pos[1];
	x1 = b->origin[0], y1 = b->origin[1];
	//y1 = yres - y1;
	int tmpx = x1-x0;
	int tmpy = y1-y0;
	Flt hypot = sqrt(tmpx*tmpx + tmpy*tmpy);
	Flt trig = sqrt(tmpy/hypot * tmpy/hypot); 
	Flt angle = ((asin(trig)*100)/1.74444444);
	//std::cout<<"bullet posxy: " << x0 << ", " << y0 << "\n";
	if(tmpx > 0 && tmpy > 0) {
		angle = 270 + angle;
		b->angle = angle;
	}
	if(tmpx > 0 && tmpy < 0) {
		angle = 270 - angle;
		b->angle = angle;
	}
	if(tmpx < 0 && tmpy < 0) {
		angle = 90 + angle;
		b->angle = angle;
	}
	if(tmpx < 0 && tmpy > 0) {
		angle = 90 - angle;
		b->angle = angle;
	}
}

void check_mouse(XEvent *e, Game *g)
{
    //Did the mouse move?
    //Was a mouse button clicked?
    //int mx = e->xbutton.x;
    //int my = e->xbutton.y;
    //
    if (e->type == ButtonRelease) {
	g->player1.is_firing = 0;
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button is down
	    //g->player1.angle=;
	    if (g->player1.bulletType == 1) {
		//g->player1.is_firing = 1;
		fire_weapon(g);
	    } else if (g->player1.bulletType == 2) {
		fire_weapon(g);
	    } else if (g->player1.bulletType == 3) {
		fire_weapon(g);
	    }
	    //std::cout<<"Mouse X:" << savex <<" , Mouse Y:" << savey <<"\n";
	    //bresenham_Ang(g->player1.pos[0], g->player1.pos[1], e->xbutton.x, e->xbutton.y, g);
	    //std::cout<<"x0*x1-y0*y1" << savey * savex <<"\n";
	}
	if (e->xbutton.button==3) {
	    //Right button is down
	}
    }
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
	//Mouse moved
	savex = e->xbutton.x;
	savey = e->xbutton.y;
	//bresenham_Ang(g->player1.pos[0], g->player1.pos[1], savex, savey, g);
	/*	if (g->bhead != NULL)
		g->bhead->angle = bulletAng(g->bhead->pos[0], g->bhead->pos[1], savex, savey);
		if (g->chead != NULL)
		g->chead->angle = bulletAng(g->chead->pos[0], g->chead->pos[1], savex, savey);
		if (g->dhead != NULL)
		g->dhead->angle = bulletAng(g->dhead->pos[0], g->dhead->pos[1], savex, savey);*/
    }
}

int check_keys(XEvent *e)
{
	//keyboard input?
	static int shift=0;
	int key = XLookupKeysym(&e->xkey, 0);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	if (e->type == KeyPress) {
		keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift=1;
			return 0;
		}
	} else {
		return 0;
	}
	if (shift){}
	switch(key) {
		case XK_Escape:
			return 1;
		case XK_f:
			break;
		case XK_s:
			break;
		case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;
	}
	return 0;
}

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

void buildZombieFragment(Zombie *ta, Zombie *a)
{
	//build ta from a
	ta->nverts = 8;
	ta->radius = a->radius / 2.0;
	Flt r2 = ta->radius / 2.0;
	Flt angle = 0.0f;
	Flt inc = (PI * 2.0) / (Flt)ta->nverts;
	for (int i=0; i<ta->nverts; i++) {
		ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
		ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
		angle += inc;
	}
	ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
	ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
	ta->pos[2] = 0.0f;
	ta->angle = 0.0;
	ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
	ta->color[0] = 0.8;
	ta->color[1] = 0.8;
	ta->color[2] = 0.7;
	ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
	ta->vel[1] = a->vel[1] + (rnd()*2.0-1.0);
	//std::cout << "frag" << std::endl;
}

/*=============== LOOT IDEAS ==================
	- 12% chance for a drop to occur...
	------------------------------------
	- 25% chance to drop double shot
	- 12% chance to drop triple shot
	-  5% chance to drop extra life
	- 40% chance to drop rapid fire
	-  8% chance to drop screen clear(won't work on boss)
	- 10% chance to drop 10s invulnerability
=============================================*/
void lootDrop(Game *g, Zombie *a)
{
	// Create a random number from 1-100
	int r1 = rand() % 100 + 1;
	std::cout<<r1<<"\n";
	if (r1%8 == 0) {
		//we have a drop... now what will it be?
		Loot *l = new Loot;
		l->pos[0] = a->pos[0];
		l->pos[1] = a->pos[1];
		lootTime(l); //start timer
		glEnable(GL_TEXTURE_2D);
		initialize_fonts();
		if (g->lhead == NULL) {
			g->lhead = l;
			g->lootcnt = 1;
			
		} else {
			l->next = g->lhead;
			g->lhead->prev = l;
			g->lhead = l;
			g->lootcnt++;
		}
		
	
		int r2 = rand() % 100 + 1;
		if (r2 >=1 && r2 <= 40) {
			//rapid fire!
			g->lhead->type = 1;
			std::cout << "RAPID FIRE DROP\n";
			g->lhead->lootbg = ppm6GetImage("./images/doubleshot.ppm");
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		} else if (r2 >=41 && r2 <= 65) {
			//double shot!
			if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
				g->lhead->type = 0;
				//dead drop, drop nothing--they have double shot or better
				std::cout << "DEAD DROP DOUBLE\n";
				return;
			} else {
				g->lhead->type = 2;
				std::cout << "DOUBLE SHOT DROP\n";
				//draw object, texture, and create collision if player touches
				g->lhead->lootbg = ppm6GetImage("./images/doubleshot.ppm");
				glGenTextures(1, &g->lhead->lootTex);
				init_textures(g->lhead->lootbg, g->lhead->lootTex);
			}
		} else if (r2 >=66 && r2 <= 77) {
			if (g->player1.bulletType == 3) {
				//dead drop, drop nothing--they have triple shot or better
				g->lhead->type = 0;
				std::cout << "DEAD DROP DOUBLE\n";
				return;
			} else {
				//triple shot!
				g->lhead->type = 3;
				std::cout << "TRIPLE SHOT DROP\n";
				g->lhead->lootbg = ppm6GetImage("./images/tripleshot.ppm");
				glGenTextures(1, &g->lhead->lootTex);
				init_textures(g->lhead->lootbg, g->lhead->lootTex);
			}
		} else if (r2 >=78 && r2 <= 87) {
			//temp invuln!
			g->lhead->type = 4;
			std::cout << "INVULNERABILITY DROP\n";
			g->lhead->lootbg = ppm6GetImage("./images/doubleshot.ppm");
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		} else if (r2 >=88 && r2 <= 95) {
			//free wave clear!
			g->lhead->type = 5;
			std::cout << "WAVE CLEAR DROP\n";
			g->lhead->lootbg = ppm6GetImage("./images/doubleshot.ppm");
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		} else {
			//1up!!!
			g->lhead->type = 6;
			std::cout << "LIFE UP DROP\n";
			g->lhead->lootbg = ppm6GetImage("./images/doubleshot.ppm");
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		}
	}
}

void updateMulti(Game *g)
{
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->player1.multiTimer, &bt);
	if (ts > 2) {
		//time to reset timer.
		g->player1.multi = 1.0;
	}
}


void updateBulletPos(Game *g, Bullet *b) 
{
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	while (b) {
		//How long has bullet been alive?
		double ts = timeDiff(&b->time, &bt);
		if (ts > 2.5) {
			//time to delete the bullet.
			Bullet *saveb = b->next;
			deleteBullet(g, b);
			b = saveb;
			g->nbullets--;
			continue;
		}
		//move the bullet
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		//Check for collision with window edges
		if (b->pos[0] < 0.0) {
			//b->pos[0] += (float)xres;
			//time to delete the bullet.
			Bullet *saveb = b->next;
			deleteBullet(g, b);
			b = saveb;
			g->nbullets--;
			continue;
		}
		else if (b->pos[0] > (float)xres) {
			//b->pos[0] -= (float)xres;
			//time to delete the bullet.
			Bullet *saveb = b->next;
			deleteBullet(g, b);
			b = saveb;
			g->nbullets--;
			continue;
		}
		else if (b->pos[1] < 0.0) {
			//b->pos[1] += (float)yres;
			//time to delete the bullet.
			Bullet *saveb = b->next;
			deleteBullet(g, b);
			b = saveb;
			g->nbullets--;
			continue;
		}
		else if (b->pos[1] > (float)yres) {
			//b->pos[1] -= (float)yres;
			//time to delete the bullet.
			Bullet *saveb = b->next;
			deleteBullet(g, b);
			b = saveb;
			g->nbullets--;
			continue;
		}
		b = b->next;
	}
}

void zomb_zomb_collision(Zombie *a)
{
	Zombie *b = a->prev;
	Flt z0, z1, zdist;
	//zombie on zombie collision
	if (b) {
		z0 = a->pos[0] - b->pos[0];
		z1 = a->pos[1] - b->pos[1];
		zdist = sqrt(z0*z0 + z1*z1);
		if(zdist < 1.5 * a->radius) {
			b->pos[0] = a->pos[0] + (z0/zdist) * a->radius * 1.01;
			b->pos[1] = a->pos[1] + (z1/zdist) * a->radius * 1.01;
		
		}
		
	}

}

void bul_zomb_collision(Game *g, Bullet *x)
{
	Flt d0,d1,dist;
	Zombie *a = g->ahead;
	while (a) {
		//is there a bullet within its radius?
		Bullet *b = x;
		while (b) {
			d0 = b->pos[0] - a->pos[0];
			d1 = b->pos[1] - a->pos[1];
			dist = (d0*d0 + d1*d1);
			if (dist < (a->radius*a->radius)) {
				//std::cout << "asteroid hit." << std::endl;
				//this asteroid is hit.
				/*if (a->radius > 20.0) {
					g->player1.score += 200;
					//break it into pieces.
					Zombie *ta = a;
					buildZombieFragment(ta, a);
					int r = rand()%10+5;
					for (int k=0; k<r; k++) {
						//get the next asteroid position in the array
						Zombie *ta = new Zombie;
						buildZombieFragment(ta, a);
						//add to front of asteroid linked list
						ta->next = g->ahead;
						if (g->ahead != NULL)
							g->ahead->prev = ta;
						g->ahead = ta;
						g->nzombies++;
					}
				} else {*/
					if(a->hitpoints == 0) {
						g->player1.multi += 0.05;
						g->player1.score += 75 * g->player1.multi;
						multitime(g);
						lootDrop(g, a);
						//asteroid is too small to break up
						//delete the asteroid and bullet
						Zombie *savea = a->next;
						deleteZombie(g, a);
						a = savea;
						g->nzombies--;
					} else {
						a->hitpoints--;
					}
				//}
				//delete the bullet...
				Bullet *saveb = b->next;
				deleteBullet(g, b);
				b = saveb;
				g->nbullets--;
				if (a == NULL)
					break;
				continue;
			}
			b = b->next;
		}
		if (a == NULL)
			break;
		a = a->next;
	}
}

void player_zomb_collision(Game *g)
{
	Flt d0,d1,dist;
	g->player1.check = 1;
	while (g->player1.check) {
		//is there a zombie within its radius?
		Zombie *z = g->ahead;
		while (z) {
			d0 = z->pos[0] - g->player1.pos[0];
			d1 = z->pos[1] - g->player1.pos[1];
			dist = (d0*d0 + d1*d1);
			if (dist < (g->player1.radius*g->player1.radius)) {
				//std::cout << "player hit" << std::endl;
				//this player is hit.
				if (!g->player1.invuln) {
					g->player1.lives--;
					std::cout<<"lives remaining: " << g->player1.lives << "\n";
					z->color[0] = 0.0;
					z->color[1] = 1.0;
					z->color[2] = 0.1;
					g->player1.multi = 1.0;
					//if (g->player1.lives == 1) print warning
					if (g->player1.lives < 1) {
						g->gameover = 1; // YOU LOSE
						std::cout<<"returning\n";
						return;
					}

					//zombieflee function??
					//DEATH ANIMATION HERE
					int cnt = 0;
					struct timespec deathTimer, counter;
					clock_gettime(CLOCK_REALTIME, &deathTimer);
					clock_gettime(CLOCK_REALTIME, &counter);
					double ts = timeDiff(&deathTimer, &counter);
					while (ts < 3.0) {
						clock_gettime(CLOCK_REALTIME, &counter);
						ts = timeDiff(&deathTimer, &counter);
						if (ts > 0.0 && !cnt) {
							std::cout<<"3... \n";
							cnt = 1;
						} else if (ts > 1.0 && cnt == 1) {
							std::cout<<"2... \n";
							cnt++;
						} else if (ts > 2.0 && cnt == 2) {
							std::cout<<"1... \n";
							cnt++;
						}
						//revival animation?Zombie *a = g->ahead;Zombie *a = g->ahead;
						Zombie *a = g->ahead;
						while (a) {
							//Try nesting everything in an if/else with a randomized bool
							//to determine if zombie is wandering or running at player?
							a->pos[0] = 0;
							a->pos[1] = 0;
							//Check for collision with window edges
							if (a->pos[0] < -100.0) {
								a->pos[0] += (float)xres+200;
							}
							else if (a->pos[0] > (float)xres+100) {
								a->pos[0] -= (float)xres+200;
							}
							else if (a->pos[1] < -100.0) {
								a->pos[1] += (float)yres+200;
							}
							else if (a->pos[1] > (float)yres+100) {
								a->pos[1] -= (float)yres+200;
							}
							a->angle += a->rotate;
							a = a->next;
						}
					}
					g->player1.pos[0] = xres/2;
					g->player1.pos[1] = yres/2;
				}

				//empower zombie? xD...
				if (z == NULL)
					return;
				g->player1.check = 0;
			}
			z = z->next;
		}
		g->player1.check = 0;
	}
}

void physics(Game *g)
{
	//Update player1 position
        g->player1.origin[0] = g->player1.pos[0];
        g->player1.origin[1] = g->player1.pos[1];

        g->player1.pos[0] += g->player1.vel[0];
        g->player1.pos[1] += g->player1.vel[1];
        //Check for collision with window edges
        //instantiate background change based on matrix
        //remove all zombies and objects and remake them
        if (g->player1.pos[0] < 0.0) {
                g->player1.pos[0] += (float)xres;
        }
        else if (g->player1.pos[0] > (float)xres) {
                g->player1.pos[0] -= (float)xres;
        }
        else if (g->player1.pos[1] < 0.0) {
                g->player1.pos[1] += (float)yres;
        }
        else if (g->player1.pos[1] > (float)yres) {
                g->player1.pos[1] -= (float)yres;
        }
        //
        //std::cout<<"Player X:" << g->player1.pos[0] <<" , Player Y:" << g->player1.pos[1] <<"\n";
        //
        //Update bullet positions
        updateBulletPos(g, g->bhead);
        if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
                updateBulletPos(g, g->chead);
                if(g->player1.bulletType == 3) {
                        updateBulletPos(g, g->dhead);
                }
        }

	updateMulti(g);	
	//
	//Update asteroid positions
	Zombie *a = g->ahead;
	while (a) {
		//Try nesting everything in an if/else with a randomized bool
		//to determine if zombie is wandering or running at player?
		zMove(g, a);
		//zomb_zomb_collision(a);
		a->pos[0] += a->vel[0];
		a->pos[1] += a->vel[1];
		//Check for collision with window edges
		if (a->pos[0] < -100.0) {
			a->pos[0] += (float)xres+200;
		}
		else if (a->pos[0] > (float)xres+100) {
			a->pos[0] -= (float)xres+200;
		}
		else if (a->pos[1] < -100.0) {
			a->pos[1] += (float)yres+200;
		}
		else if (a->pos[1] > (float)yres+100) {
			a->pos[1] -= (float)yres+200;
		}
		a->angle += a->rotate;
		a = a->next;
	}
        //Zombie collision with bullets?
        //If collision detected:
        //     1. delete the bullet
        //     2. break the asteroid into pieces
        //        if asteroid small, delete it
        bul_zomb_collision(g, g->bhead);
        if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
                bul_zomb_collision(g, g->chead);
                if (g->player1.bulletType == 3) {
                        bul_zomb_collision(g, g->dhead);
                }
        }
	
	//Player collision with zombies
	player_zomb_collision(g);
	if(g->gameover) {
		std::cout<<"returning again\n";
		return;
	}
	//---------------------------------------------------
	//check keys pressed now
	//NOTE:: ANGLE CHECKED COUNTER CLOCKWISE

	last_Position_S = g->player1.angle;
	//BREAK if attempting to move opposite directions at same time
	if (((keys[XK_Left] || keys[XK_a]) && (keys[XK_Right] || keys[XK_d])) ||
			((keys[XK_Up]   || keys[XK_w]) && (keys[XK_Down]  || keys[XK_s]))) {
		//convert player1 angle to radians
		//convert angle to a vector
		g->player1.vel[0] = 0;
		g->player1.vel[1] = 0;
		g->player1.angle = last_Position_S;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;
	}
	else if ((keys[XK_Up] || keys[XK_w]) && (keys[XK_Left] || keys[XK_a])) {
		normalize(g->player1.vel);
		g->player1.vel[0] = -4;
		g->player1.vel[1] = 4;
	}
	else if ((keys[XK_Down] || keys[XK_s]) && (keys[XK_Left] || keys[XK_a])) {
		normalize(g->player1.vel);
		g->player1.vel[0] = -4;
		g->player1.vel[1] = -4;
	}
	else if ((keys[XK_Down] || keys[XK_s]) && (keys[XK_Right] || keys[XK_d])) {
		normalize(g->player1.vel);
		g->player1.vel[0] = 4;
		g->player1.vel[1] = -4;

	}
	else if ((keys[XK_Up] || keys[XK_w]) && (keys[XK_Right] || keys[XK_d])) {
		normalize(g->player1.vel);
		g->player1.vel[0] = 4;
		g->player1.vel[1] = 4;

	}
	else if (keys[XK_Left] || keys[XK_a]) {
		normalize(g->player1.vel);
		g->player1.vel[0] = -8;

	}
	else if (keys[XK_Right] || keys[XK_d]) {
		normalize(g->player1.vel);
		g->player1.vel[0] = 8;

	}
	else if (keys[XK_Up] || keys[XK_w]) {
		normalize(g->player1.vel);
		g->player1.vel[1] = 8;

	}
	else if (keys[XK_Down] || keys[XK_s]) {
		normalize(g->player1.vel);
		g->player1.vel[1] = -8;

	}
	else {
		//convert player1 angle to radians
		//convert angle to a vector
		g->player1.vel[0] = 0;
		g->player1.vel[1] = 0;
		g->player1.angle = last_Position_S;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;

	}
	bresenham_Ang(g);
	if (keys[XK_i]) {
		g->player1.invuln++;
		if (g->player1.invuln == 2)
			g->player1.invuln = 0;
		keys[XK_i] = 0;
	}

	if (keys[XK_space]) {
		fire_weapon(g);
	}
	if (g->player1.is_firing) {
		fire_weapon(g);
	}

	if (keys[XK_1]) {
		g->player1.bulletType = 1;
	
	} else if (keys[XK_2]) {
		g->player1.bulletType = 2;
	
	} else if (keys[XK_3]) {
		g->player1.bulletType = 3;
	}
}

void lootTime(Loot *l)
{
	struct timespec lt;
	clock_gettime(CLOCK_REALTIME, &lt);
	double ts = timeDiff(&l->lootTimer, &lt);
	if (ts > 0.1) {
		timeCopy(&l->lootTimer, &lt);
	}
}

void multitime(Game *g)
{
	struct timespec mt;
	clock_gettime(CLOCK_REALTIME, &mt);
	double ts = timeDiff(&g->player1.multiTimer, &mt);
	if (ts > 0.1) {
		timeCopy(&g->multiTimer, &mt);
		timeCopy(&g->player1.multiTimer, &mt);
	}
}

void fire_weapon(Game *g) 
{
	//a little time between each bullet
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->bulletTimer, &bt);
	if (ts > 0.1) {
		switch (g->player1.bulletType) {
			case 1: {
					timeCopy(&g->bulletTimer, &bt);
					//shoot a bullet...
					Bullet *b = new Bullet;
					b->type = 1;
					timeCopy(&b->time, &bt);
					b->pos[0] = g->player1.pos[0];
					b->pos[1] = g->player1.pos[1];
					b->origin[0] = g->player1.pos[0];
					b->origin[1] = g->player1.pos[1];
					b->origin[2] = g->player1.pos[2];
					b->vel[0] = 0;
					b->vel[1] = 0;
					b->angle = g->player1.angle;
					//convert player1 angle to radians
					Flt rad = ((g->player1.angle+90.0) / 360.0f) * PI * 2.0;
					//convert angle to a vector
					Flt xdir = cos(rad);
					Flt ydir = sin(rad);
					b->pos[0] += xdir*20.0f;
					b->pos[1] += ydir*20.0f;
					b->vel[0] += xdir*18.0f + rnd()*0.1;
					b->vel[1] += ydir*18.0f + rnd()*0.1;
					b->color[0] = 1.0f;
					b->color[1] = 1.0f;
					b->color[2] = 1.0f;
					//add to front of bullet linked list
					b->next = g->bhead;
					if (g->bhead != NULL)
						g->bhead->prev = b;
					g->bhead = b;
					g->nbullets++;
					break;
				}
			case 2: {
					timeCopy(&g->bulletTimer, &bt);
					//shoot a bullet...
					Bullet *b = new Bullet;
					b->type = 1;
					Bullet *c = new Bullet;
					c->type = 2;
					timeCopy(&b->time, &bt);
					timeCopy(&c->time, &bt);
					b->pos[0] = g->player1.pos[0];
					b->pos[1] = g->player1.pos[1];
					b->origin[0] = g->player1.pos[0];
					b->origin[1] = g->player1.pos[1];
					b->vel[0] = 0;
					b->vel[1] = 0;
					c->pos[0] = g->player1.pos[0];
					c->pos[1] = g->player1.pos[1];
					c->origin[0] = g->player1.pos[0];
					c->origin[1] = g->player1.pos[1];
					c->vel[0] = 0;
					c->vel[1] = 0;
					//convert player1 angle to radians
					Flt rad1 = ((g->player1.angle+110.0) / 360.0f) * PI * 2.0;
					Flt rad2 = ((g->player1.angle+70.0) / 360.0f) * PI * 2.0;
					//convert angle to a vector
					Flt xdir1 = cos(rad1);
					Flt ydir1 = sin(rad1);
					Flt xdir2 = cos(rad2);
					Flt ydir2 = sin(rad2);
					b->pos[0] += xdir1*10.0f;
					b->pos[1] += ydir1*10.0f;
					normalize(b->vel);
					b->vel[0] = xdir1*16 + rnd()*0.1;
					b->vel[1] = ydir1*16 + rnd()*0.1;
					b->color[0] = 1.0f;
					b->color[1] = 1.0f;
					b->color[2] = 1.0f;

					c->pos[0] += xdir2*20.0f;
					c->pos[1] += ydir2*20.0f;
					normalize(c->vel);
					c->vel[0] = xdir2*16 + rnd()*0.1;
					c->vel[1] = ydir2*16 + rnd()*0.1;
					c->color[0] = 1.0f;
					c->color[1] = 1.0f;
					c->color[2] = 1.0f;
					//add to front of bullet linked list
					b->next = g->bhead;
					c->next = g->chead;
					if (g->bhead != NULL) {
						g->bhead->prev = b;
					}
					if (g->chead != NULL) {
						g->chead->prev = c;
					}
					g->bhead = b;
					g->chead = c;
					g->nbullets+=2;
					break;
				}
			case 3: {
					timeCopy(&g->bulletTimer, &bt);
					//shoot a bullet...
					Bullet *b = new Bullet;
					b->type = 1;
					Bullet *c = new Bullet;
					c->type = 2;
					Bullet *d = new Bullet;
					d->type = 3;
					timeCopy(&b->time, &bt);
					timeCopy(&c->time, &bt);
					timeCopy(&d->time, &bt);
					b->pos[0] = g->player1.pos[0];
					b->pos[1] = g->player1.pos[1];
					b->origin[0] = g->player1.pos[0];
					b->origin[1] = g->player1.pos[1];
					b->vel[0] = 0;
					b->vel[1] = 0;
					c->pos[0] = g->player1.pos[0];
					c->pos[1] = g->player1.pos[1];
					c->origin[0] = g->player1.pos[0];
					c->origin[1] = g->player1.pos[1];
					c->vel[0] = 0;
					c->vel[1] = 0;
					d->pos[0] = g->player1.pos[0];
					d->pos[1] = g->player1.pos[1];
					d->origin[0] = g->player1.pos[0];
					d->origin[1] = g->player1.pos[1];
					d->vel[0] = 0;
					d->vel[1] = 0;
					//convert player1 angle to radians
					Flt rad = ((g->player1.angle+90.0) / 360.0f) * PI * 2.0;
					Flt rad1 = ((g->player1.angle+110.0) / 360.0f) * PI * 2.0;
					Flt rad2 = ((g->player1.angle+70.0) / 360.0f) * PI * 2.0;
					//convert angle to a vector
					Flt xdir = cos(rad);
					Flt ydir = sin(rad);
					Flt xdir1 = cos(rad1);
					Flt ydir1 = sin(rad1);
					Flt xdir2 = cos(rad2);
					Flt ydir2 = sin(rad2);
					b->pos[0] += xdir1*10.0f;
					b->pos[1] += ydir1*10.0f;
					normalize(b->vel);
					b->vel[0] = xdir1*16 + rnd()*0.1;
					b->vel[1] = ydir1*16 + rnd()*0.1;
					b->color[0] = 1.0f;
					b->color[1] = 1.0f;
					b->color[2] = 1.0f;

					c->pos[0] += xdir2*20.0f;
					c->pos[1] += ydir2*20.0f;
					normalize(c->vel);
					c->vel[0] = xdir2*16 + rnd()*0.1;
					c->vel[1] = ydir2*16 + rnd()*0.1;
					c->color[0] = 1.0f;
					c->color[1] = 1.0f;
					c->color[2] = 1.0f;

					d->pos[0] += xdir*20.0f;
					d->pos[1] += ydir*20.0f;
					normalize(d->vel);
					d->vel[0] = xdir*16 + rnd()*0.1;
					d->vel[1] = ydir*16 + rnd()*0.1;
					d->color[0] = 1.0f;
					d->color[1] = 1.0f;
					d->color[2] = 1.0f;
					//add to front of bullet linked list
					b->next = g->bhead;
					c->next = g->chead;
					d->next = g->dhead;
					if (g->bhead != NULL) {
						g->bhead->prev = b;
					}
					if (g->chead != NULL) {
						g->chead->prev = c;
					}
					if (g->dhead != NULL) {
						g->dhead->prev = d;
					}
					g->bhead = b;
					g->chead = c;
					g->dhead = d;
					g->nbullets+=3;
					break;
				}
		}
	}
}

void render_StartScreen(Game *g)
{
	Rect r,s;
	glClear(GL_COLOR_BUFFER_BIT);
	sscreen_background(bgTexture0, 1.0, 1.0, 1.0, 1.0);
	//
	//XDrawString(dis,win,gc,x,y, string, strlen(string));
	r.bot = yres - yres*0.7;
	r.left = xres - xres*0.5;
	r.center = 1;
	ggprint16(&r, 32, 0x00ff00ff, "START GAME");
	ggprint16(&r, 32, 0x00ff00ff, "OPTIONS");
	ggprint16(&r, 32, 0x00ff00ff, "HIGH SCORES");
	//...

	switch (g->current_selection) {
		case 1: {
				s.bot = yres - yres*0.7;
				s.left = xres - xres*0.5;
				s.center = 1;
				ggprint16(&s, 32, 0x00ffffff, "[                   ]");
				break;
			}
		case 2: {
				s.bot = yres - yres*0.7 - 32;
				s.left = xres - xres*0.5;
				s.center = 1;
				ggprint16(&s, 32, 0x00ffffff, "[           ]");
				break;
			}
		case 3: {
				s.bot = yres - yres*0.7 - 64;
				s.left = xres - xres*0.5;
				s.center = 1;
				ggprint16(&s, 32, 0x00ffffff, "[                    ]");
				break;
			}
		case 0: {   // Enter was pressed
				switch (g->old_selection) {
					case 1: {
							g->startScreen = 0;
							g->current_selection = g->old_selection;
							break;
						}
					case 2: {
							//options...
							g->current_selection = g->old_selection;
							break;
						}
					case 3: {
							//High Scores page...
							g->current_selection = g->old_selection;
							break;
						}
				}
			}
	}


	if ((keys[XK_Down] || keys[XK_s]) && (g->current_selection < 3)) {
		g->current_selection++;
		keys[XK_Down] = 0;
		keys[XK_s] = 0;
	} else if ((keys[XK_Up] || keys[XK_w]) && (g->current_selection > 1)) {
		g->current_selection--;
		keys[XK_Up] = 0;
		keys[XK_w] = 0;
	} else if (keys[XK_Return] || keys[XK_space]) {
		g->old_selection = g->current_selection;
		g->current_selection = 0;
	}
}

void rendergameoverScreen(Game *g)
{
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	sscreen_background(gameoverTex, 1.0, 1.0, 1.0, 1.0);
	//glClearColor(1.0, 1.0, 1.0, 1.0);
	//
	r.bot = yres*0.7;
	r.left = xres*0.5;
	r.center = 1;
	ggprint12(&r, 32, 0x00ff00ff, "Your SCORE IS: %i, zone: %i, wave: %i", 
			g->player1.score, g->zcnt, g->wcnt);
	ggprint16(&r, 32, 0x00ff00ff, "Enter Name");
}

void renderscoreScreen(Game *g)
{
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//
	r.bot = yres*0.7;
	r.left = xres*0.5;
	r.center = 1;
	ggprint16(&r, 32, 0x00ff00ff, "GAME OVER");
	ggprint12(&r, 24, 0x00ff00ff, "A Zombie Ate Your Brains");
	ggprint12(&r, 32, 0x00ff00ff, "Your SCORE IS: %i", g->player1.score);
	ggprint16(&r, 32, 0x00ff00ff, "Try Again");
	ggprint16(&r, 32, 0x00ff00ff, "Exit");
}

void render(Game *g)
{
	//std::cout<<"multi: " << g->player1.multi << "\n";
	//float wid;
	if (g->nzombies == 0) {
		init(g);
		std::cout<<"here\n";
	}

	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	sscreen_background(g->zhead->zTexture, 1.0, 1.0, 1.0, 1.0);
	
	//draw loot
	Loot *l = new Loot;
	l = g->lhead;
	while (l != NULL) {
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		double ts = timeDiff(&l->lootTimer, &bt);
		if (ts > 3) {
			//MAKE IT BLINK!!!!!!!!
		}
		if (ts > 5 || l->type == 0) {
			//time to delete loot
			deleteLoot(g,l);
		} else {
			lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
		}
		l = l->next;
		
	}
	//
	r.bot = yres - 64;
	r.left = 10;
	r.center = 0;
	ggprint16(&r, 32, 0x0011ff11, "PLAYER 1 SCORE: %i", g->player1.score);
	ggprint16(&r, 16, 0x00ff1111, "MULTI         : %.3g", g->player1.multi);

	ggprint8b(&r, 16, 0x00ff1111, "Zone %i, Wave %i", g->zcnt, g->wcnt);
	ggprint8b(&r, 16, 0x00ff1111, "n bullets: %i", g->nbullets);
	ggprint8b(&r, 16, 0x00ff1111, "Zombies left: %i", g->nzombies);
	//-------------------------------------------------------------------------
	//Draw the player1
	glColor3fv(g->player1.color);
	glPushMatrix();
	glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
	//float angle = atan2(player1.dir[1], player1.dir[0]);
	//std::cout<<"angle = " << g->player1.angle << std::endl;
	glBindTexture(GL_TEXTURE_2D, player1Tex);
	glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glBegin(GL_QUADS);
		//float w = g->player1.width;
		float w = 25.0f;
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
	//glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	//glVertex2f(-10.0f, -10.0f);
	//glVertex2f(  0.0f, 20.0f);
	//glVertex2f( 10.0f, -10.0f);
	glVertex2f(-12.0f, -10.0f);
	glVertex2f(  0.0f, 20.0f);
	glVertex2f(  0.0f, -6.0f);
	glVertex2f(  0.0f, -6.0f);
	glVertex2f(  0.0f, 20.0f);
	glVertex2f( 12.0f, -10.0f);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	if (g->player1.invuln) {
		float col[3] = {0.0, 0.0, 0.0};
		glColor3fv(col);
		glPushMatrix();
		glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
		glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
		glBegin(GL_TRIANGLES);
		glVertex2f(-12.0f, -10.0f);
		glVertex2f(  0.0f, 20.0f);
		glVertex2f(  0.0f, -6.0f);
		glVertex2f(  0.0f, -6.0f);
		glVertex2f(  0.0f, 20.0f);
		glVertex2f( 12.0f, -10.0f);
		glEnd();
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(0.0f, 0.0f);
		glEnd();
		glPopMatrix();
		//std::cout<< "blinking!" << "\n";
	}

	if (keys[XK_Up]) {
		int i;
		//draw thrust
		Flt rad = ((g->player1.angle+90.0) / 360.0f) * PI * 2.0;
		//convert angle to a vector
		Flt xdir = cos(rad);
		Flt ydir = sin(rad);
		Flt xs,ys,xe,ye,r;
		glBegin(GL_LINES);
		for (i=0; i<16; i++) {
			xs = -xdir * 11.0f + rnd() * 4.0 - 2.0;
			ys = -ydir * 11.0f + rnd() * 4.0 - 2.0;
			r = rnd()*40.0+40.0;
			xe = -xdir * r + rnd() * 18.0 - 9.0;
			ye = -ydir * r + rnd() * 18.0 - 9.0;
			glColor3f(rnd()*.3+.7, rnd()*.3+.7, 0);
			glVertex2f(g->player1.pos[0]+xs,g->player1.pos[1]+ys);
			glVertex2f(g->player1.pos[0]+xe,g->player1.pos[1]+ye);
		}
		glEnd();
	}
	//-------------------------------------------------------------------------
	//Draw the asteroids
	{
		Zombie *a = g->ahead;
		while (a) {
			//Log("draw asteroid...\n");
			glColor3fv(a->color);
			glPushMatrix();
			glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
			glBindTexture(GL_TEXTURE_2D, zombieTex);
			glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f);
			glBegin(GL_QUADS);
				// float w is zombie size
				float w = 25.0f;
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
				glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
				glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
			glEnd();
			glPopMatrix();
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_ALPHA_TEST);
			//glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
			glBegin(GL_QUADS);
			glVertex2i(-20,-20);
			glVertex2i(-20, 20);
			glVertex2i( 20, 20);
			glVertex2i( 20,-20);
			
			//Log("%i verts\n",a->nverts);
			//for (int j=0; j<a->nverts; j++) {
			//	glVertex2f(a->vert[j][0], a->vert[j][1]);
			//}
	
			glEnd();
			//glBegin(GL_LINES);
			//	glVertex2f(0,   0);
			//	glVertex2f(a->radius, 0);
			//glEnd();
			glPopMatrix();
			glColor3f(1.0f, 0.0f, 0.0f);
//			glBegin(GL_POINTS);
//			glVertex2f(a->pos[0], a->pos[1]);
			glBegin(GL_POINTS);
			glVertex2f(a->pos[0], a->pos[1]);
			glEnd();
			a = a->next;
		}
	}
	//std::cout<<"player angle: " << g->player1.angle << "\n";
	//-------------------------------------------------------------------------
	//Draw the bullets
		//std::cout<<"player posxy: " << g->player1.pos[0] << ", " << g->player1.pos[1] << ", " << g->player1.pos[2] << "\n";
		if (g->bhead != NULL) {
			bulletDraw(g->bhead);
			if (g->chead != NULL) {
				if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
					bulletDraw(g->chead);
					if (g->player1.bulletType == 3 && g->dhead != NULL) {
						bulletDraw(g->dhead);
					}
				}
			}
		}
	//Draw Loot
	//sscreen_background(g->zhead->zTexture, 1.0, 1.0, 1.0, 1.0);
	
}

void lootDraw(GLuint tex, Loot *l, float r, float g, float b, float alph)
{
		glClearColor(r, g, b, alph);
		//glClear(GL_COLOR_BUFFER_BIT);
		//draw textured quad
		//float wid = 120.0f;
		//glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(l->pos[0]-14, l->pos[1]-14);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(l->pos[0]-14, l->pos[1]+14);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(l->pos[0]+14, l->pos[1]+14);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(l->pos[0]+14, l->pos[1]-14);
		glEnd();
}

void bulletDraw(Bullet *b)
{
	while (b) {
		//Log("draw bullet...\n");
		//glColor3f(1.0, 1.0, 1.0);
		glColor3fv(b->color);
		glPushMatrix();
		/*glBegin(GL_POINTS);
		  glVertex2f(b->pos[0],      b->pos[1]);
		  glVertex2f(b->pos[0]-1.0f, b->pos[1]);
		  glVertex2f(b->pos[0]+1.0f, b->pos[1]);
		  glVertex2f(b->pos[0],      b->pos[1]-1.0f);
		  glVertex2f(b->pos[0],      b->pos[1]+1.0f);
		  glColor3f(0.8, 0.8, 0.8);
		  glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
		  glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
		  glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
		  glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
		  glEnd();*/
		//glTranslatef(b->origin[0], b->origin[1], b->origin[2]);
		//glRotatef(1.0f, 0.0f, 0.0f, 1.0f);
		//std::cout<<"bullet angle: " << b->angle << "\n";
		//std::cout<<"bullet posxy: " << b->pos[0] << ", " << b->pos[1] << ", " << b->pos[2] << "\n";

		glBegin(GL_QUADS);
		glVertex2f(b->pos[0]-8.0f, b->pos[1]+2.0f);
		glVertex2f(b->pos[0]+8.0f, b->pos[1]+2.0f);
		glVertex2f(b->pos[0]+8.0f, b->pos[1]-2.0f);
		glVertex2f(b->pos[0]-8.0f, b->pos[1]-2.0f);
		/*glColor3f(0.8, 0.8, 0.8);
		  glVertex2f(b->pos[0]-8.0f, b->pos[1]-8.0f);
		  glVertex2f(b->pos[0]-8.0f, b->pos[1]+8.0f);
		  glVertex2f(b->pos[0]+8.0f, b->pos[1]-8.0f);
		  glVertex2f(b->pos[0]+8.0f, b->pos[1]+8.0f);*/
		glEnd();
		glPopMatrix();


		b = b->next;
	}
}	

//taken from cmps371 work ~bware
/*void check_images(void)
  {
//Pull texture off a PPM image  
int i, j;
Ppmimage *image = ppm6GetImage("drock057.ppm");
GLubyte *c = (GLubyte *)image->data;

for (i = 0; i < checkImageHeight; i++) {
for (j = 0; j < checkImageWidth; j++) {
image1[i][j][0] = (GLubyte) *c;
image1[i][j][1] = (GLubyte) *(c+1);
image1[i][j][2] = (GLubyte) *(c+2);
image1[i][j][3] = (GLubyte) 255;
c+=3;
}
}
}
 */

void sscreen_background(GLuint tex, float r, float g, float b, float alph)
{
	
	glClearColor(r, g, b, alph);
	glClear(GL_COLOR_BUFFER_BIT);
	//draw textured quad
	//float wid = 120.0f;
	glColor3f(1.0, 1.0, 1.0);
	//glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
	glEnd();
}


/*int fib(int n)
  {
  if (n <= 1)
  return n;
  return fib (n-1) + fib(n-2);
  }
 */
