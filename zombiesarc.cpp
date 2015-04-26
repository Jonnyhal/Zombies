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
// Zombie spawn/movement algorithm    []
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
int bulletType = 0;
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

//
GLuint bgTexture0;

struct Player {
	Vec dir;
	Vec pos;
	Vec vel;
	int score;
	float angle;
	float color[3];
	Player() {
		VecZero(dir);
		pos[0] = (Flt)(xres/2);
		pos[1] = (Flt)(yres/2);
		pos[2] = 0.0f;
		VecZero(vel);
		angle = 0.0;
		color[0] = 1.0;
		color[1] = 1.0;
		color[2] = 1.0;
	}
};

struct Bullet {
	Vec pos;
	Vec vel;
	float color[3];
	struct timespec time;
	struct Bullet *prev;
	struct Bullet *next;
	Bullet() {
		prev = NULL;
		next = NULL;
	}
};

struct Asteroid {
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
	Asteroid() {
		prev = NULL;
		next = NULL;
	}
	
};

struct Game {
	Player player1;
	Asteroid *ahead;
	Bullet *bhead;
	int nasteroids;
	int nbullets;
	int startScreen;
	int current_selection;
	int old_selection;
	struct timespec bulletTimer;
	Game() {
		ahead = NULL;
		bhead = NULL;
		nasteroids = 0;
		nbullets = 0;
	}
};


int keys[65536];

//function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_resize(XEvent *e);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e);
void init(Game *g);
void init_sounds(void);
void physics(Game *game);
void fire_weapon(Game *game);
void render(Game *game);
void bresenham_Ang(int p1, int p2, int p3, int p4, Game *g);
void render_StartScreen(Game *game);
void sscreen_background(void);
int fib(int n);

int main(void)
{
	logOpen();
	initXWindows();
	init_opengl();
	Game game;
	game.current_selection = 1;
	game.startScreen = 1;
	int done=0;
	int donesscreen;
	//sscreen_background(&game);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	while (game.startScreen) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, &game);
			if((donesscreen = check_keys(&e))) {//NOT comparing, setting and checking value for 0/1
				game.startScreen = 0;
				done = 1;
			}
		}
		render_StartScreen(&game);
		glXSwapBuffers(dpy, win);
	}
	//cleanupXWindows();
	//cleanup_fonts();
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//init_opengl();
	init(&game);
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	game.player1.score = 0;
	//we should make a player initialization function
	while (!done) {
		while (XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_mouse(&e, &game);
			done = check_keys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics(&game);
			physicsCountdown -= physicsRate;
		}
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	cleanup_fonts();
	logClose();
	return 0;
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
	glClearColor(1.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();

	//Load image files
	background0   = ppm6GetImage("./images/ssbg.ppm");


	//Generate Textures
	glGenTextures(1, &bgTexture0);



	//background
	glBindTexture(GL_TEXTURE_2D, bgTexture0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3,background0->width, background0->height,
			0, GL_RGB, GL_UNSIGNED_BYTE, background0->data);
	//add transparency?


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
	//build 10 asteroids...
	for (int j=0; j<20; j++) {
		Asteroid *a = new Asteroid;
		a->nverts = 8;
		a->radius = 20.0;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;
		Flt inc = (PI * 2.0) / (Flt)a->nverts;
		for (int i=0; i<a->nverts; i++) {
			a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
			a->vert[i][1] = cos(angle) * (r2 + rnd() * a->radius);
			angle += inc;
		}
		a->angle = 0.0;
		std::cout << "asteroid" << std::endl;
		//left part of screen 3/4 down from top
		if((j%5)==0){
			a->pos[0] = (Flt)(0);
			a->pos[1] = (Flt)(yres*0.65);
			a->pos[2] = 0.0f;
			a->color[0] = 0.8;
			a->color[1] = 0.8;
			a->color[2] = 0.7;
			a->vel[0] = (Flt)(rnd()*2.0);
			a->vel[1] = (Flt)(0);
		}
		//left part of screen 1/4 down from the top
		if((j%5)==1){
			a->pos[0] = (Flt)(0);
			a->pos[1] = (Flt)(yres*0.25);
			a->pos[2] = 0.0f;
			a->color[0] = 0.8;
			a->color[1] = 0.8;
			a->color[2] = 0.7;
			a->vel[0] = (Flt)(rnd()*2.0);
			a->vel[1] = (Flt)(0);
		}
		//bottom
		if((j%5)==2){
			a->pos[0] = (Flt)(xres*0.25);
			a->pos[1] = (Flt)(0);
			a->pos[2] = 0.0f;
			a->color[0] = 0.8;
			a->color[1] = 0.8;
			a->color[2] = 0.7;
			a->vel[0] = (Flt)(0);
			a->vel[1] = (Flt)(rnd()*(2.0));
		}
		//right middle
		if((j%5)==3){
			a->pos[0] = (Flt)(xres);
			a->pos[1] = (Flt)(yres*0.5);
			a->pos[2] = 0.0f;
			a->color[0] = 0.8;
			a->color[1] = 0.8;
			a->color[2] = 0.7;
			a->vel[0] = (Flt)(rnd()*(-2.0));
			a->vel[1] = (Flt)(rnd()*(0));
		}
		//top
		if((j%5)==4){
			a->pos[0] = (Flt)(xres*0.65);
			a->pos[1] = (Flt)(yres);
			a->pos[2] = 0.0f;
			a->color[0] = 0.8;
			a->color[1] = 0.8;
			a->color[2] = 0.7;
			a->vel[0] = (Flt)(0);
			a->vel[1] = (Flt)(rnd()*(-2.0));
		}	
		//add to front of linked list
		a->next = g->ahead;
		if (g->ahead != NULL)
			g->ahead->prev = a;
		g->ahead = a;
		g->nasteroids++;
	}
	clock_gettime(CLOCK_REALTIME, &g->bulletTimer);
	memset(keys, 0, 65536);
}

void normalize(Vec v) {
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
void bresenham_Ang(int x0, int y0, int x1, int y1, Game *g) 
{
	// I used a TON of divides... maybe look to optimize? ~bware
	// fixed... mostly  ~bware
	//Calculate where to angle and shoot based on pointer
	//int x, y, xDiff, yDiff, err;
	y1 = yres - y1;
	int tmpx = x1-x0;
	int tmpy = y1-y0;
	Flt hypot = sqrt(tmpx*tmpx + tmpy*tmpy);
	Flt trig = sqrt(tmpy/hypot * tmpy/hypot); 
	Flt angle = ((asin(trig)*100)/1.74444444);

	if(tmpx > 0 && tmpy > 0) {

		angle = 270 + angle;
		//std::cout<<"TOP RIGHT trig asin: " << angle <<"\n";
		//std::cout<<"Mous Pos:X:  " << x1 << " ,Y:  "<< y1<<"\n";
		//std::cout<<"Player Pos:X:  " << x0 << " ,Y:  "<< y0<<"\n";
		//std::cout<<" x1 - x0 =  " << tmpx << " ,y1-. = "<< tmpy <<"\n";
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
	}
	if(tmpx > 0 && tmpy < 0) {
		angle = 270 - angle;
		//std::cout<<"BOT RIGHT trig asin: " << angle <<"\n";
		//std::cout<<"Mous Pos:X:  " << x1 << " ,Y:  "<< y1<<"\n";
		//std::cout<<"Player Pos:X:  " << x0 << " ,Y:  "<< y0<<"\n";
		//std::cout<<" x1 - x0 =  " << tmpx << " ,y1-. = "<< tmpy <<"\n";
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;
	}
	if(tmpx < 0 && tmpy < 0) {
		angle = 90 + angle;
		//std::cout<<"BOT LEFT trig asin: " << angle <<"\n";
		//std::cout<<"Mous Pos:X:  " << x1 << " ,Y:  "<< y1<<"\n";
		//std::cout<<"Player Pos:X:  " << x0 << " ,Y:  "<< y0<<"\n";
		//std::cout<<" x1 - x0 =  " << tmpx << " ,y1-. = "<< tmpy <<"\n";
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;
		if (g->player1.angle >= 360.0f)
			g->player1.angle -= 360.0f;
	}
	if(tmpx < 0 && tmpy > 0) {
		angle = 90 - angle;
		//std::cout<<"TOP LEFT trig asin: " << angle <<"\n";
		//std::cout<<"Mous Pos:X:  " << x1 << " ,Y:  "<< y1<<"\n";
		//std::cout<<"Player Pos:X:  " << x0 << " ,Y:  "<< y0<<"\n";
		//std::cout<<" x1 - x0 =  " << tmpx << " ,y1-. = "<< tmpy <<"\n";
		g->player1.angle = angle;
		last_Position_S = g->player1.angle;

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
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
			//g->player1.angle=;

			fire_weapon(g);
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
		bresenham_Ang(g->player1.pos[0], g->player1.pos[1], savex, savey, g);
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

void deleteBullet(Game *g, Bullet *node)
{
	//remove a node from linked list
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
}

void deleteAsteroid(Game *g, Asteroid *node)
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

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
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

void physics(Game *g)
{
	Flt d0,d1,dist;
	//Update player1 position
	g->player1.pos[0] += g->player1.vel[0];
	g->player1.pos[1] += g->player1.vel[1];
	//Check for collision with window edges
	//instantiate background change based on matrix
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
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	Bullet *b = g->bhead;
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
	//
	//Update asteroid positions
	Asteroid *a = g->ahead;
	while (a) {
		//Try nesting everything in an if/else with a randomized bool
		//to determine if zombie is wandering or running at player?
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
	//
	//Asteroid collision with bullets?
	//If collision detected:
	//     1. delete the bullet
	//     2. break the asteroid into pieces
	//        if asteroid small, delete it
	a = g->ahead;
	while (a) {
		//is there a bullet within its radius?
		Bullet *b = g->bhead;
		while (b) {
			d0 = b->pos[0] - a->pos[0];
			d1 = b->pos[1] - a->pos[1];
			dist = (d0*d0 + d1*d1);
			if (dist < (a->radius*a->radius)) {
				//std::cout << "asteroid hit." << std::endl;
				//this asteroid is hit.
				if (a->radius > 20.0) {
					g->player1.score += 200;
					//break it into pieces.
					Asteroid *ta = a;
					buildAsteroidFragment(ta, a);
					int r = rand()%10+5;
					for (int k=0; k<r; k++) {
						//get the next asteroid position in the array
						Asteroid *ta = new Asteroid;
						buildAsteroidFragment(ta, a);
						//add to front of asteroid linked list
						ta->next = g->ahead;
						if (g->ahead != NULL)
							g->ahead->prev = ta;
						g->ahead = ta;
						g->nasteroids++;
					}
				} else {
					g->player1.score += 75;
					a->color[0] = 1.0;
					a->color[1] = 0.1;
					a->color[2] = 0.1;
					//asteroid is too small to break up
					//delete the asteroid and bullet
					Asteroid *savea = a->next;
					deleteAsteroid(g, a);
					a = savea;
					g->nasteroids--;
				}
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
	//---------------------------------------------------
	//check keys pressed now
	//NOTE:: ANGLE CHECKED COUNTER CLOCKWISE

	if(keys) {	
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
		bresenham_Ang(g->player1.pos[0], g->player1.pos[1], savex, savey, g);
		if (keys[XK_space]) {
			fire_weapon(g);
		}
	}
}



void fire_weapon(Game *g) 
{
	//a little time between each bullet
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->bulletTimer, &bt);
	if (ts > 0.1) {
		switch (bulletType) {
			case 0: {
					timeCopy(&g->bulletTimer, &bt);
					//shoot a bullet...
					Bullet *b = new Bullet;
					timeCopy(&b->time, &bt);
					b->pos[0] = g->player1.pos[0];
					b->pos[1] = g->player1.pos[1];
					b->vel[0] = 0;
					b->vel[1] = 0;
					//convert player1 angle to radians
					Flt rad = ((g->player1.angle+90.0) / 360.0f) * PI * 2.0;
					//convert angle to a vector
					Flt xdir = cos(rad);
					Flt ydir = sin(rad);
					b->pos[0] += xdir*20.0f;
					b->pos[1] += ydir*20.0f;
					b->vel[0] += xdir*9.0f + rnd()*0.1;
					b->vel[1] += ydir*9.0f + rnd()*0.1;
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
			case 1: {
					timeCopy(&g->bulletTimer, &bt);
					//shoot a bullet...
					Bullet *b = new Bullet;
					timeCopy(&b->time, &bt);
					b->pos[0] = g->player1.pos[0];
					b->pos[1] = g->player1.pos[1];
					b->vel[0] = 0;
					b->vel[1] = 0;
					//convert player1 angle to radians
					Flt rad = ((g->player1.angle+90.0) / 360.0f) * PI * 2.0;
					//convert angle to a vector
					Flt xdir = cos(rad);
					Flt ydir = sin(rad);
					b->pos[0] += xdir*20.0f;
					b->pos[1] += ydir*20.0f;
					b->vel[0] += xdir*9.0f + rnd()*0.1;
					b->vel[1] += ydir*9.0f + rnd()*0.1;
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
		}
	}
}

void render_StartScreen(Game *g)
{
	Rect r,s;
	glClear(GL_COLOR_BUFFER_BIT);
	sscreen_background();
	//
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

void render(Game *g)
{
	//float wid;
	Rect r;
	glClear(GL_COLOR_BUFFER_BIT);
	sscreen_background(); //CHANGE THIS TO GAME BACKGROUND FUNCTION! ~bware
	//
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "PLAYER 1 SCORE: %i", g->player1.score);
	//ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g->nbullets);
	ggprint8b(&r, 16, 0x00ffff00, "Zombies left: %i", g->nasteroids);
	//-------------------------------------------------------------------------
	//Draw the player1
	glColor3fv(g->player1.color);
	glPushMatrix();
	glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
	//float angle = atan2(player1.dir[1], player1.dir[0]);
	glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
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
		Asteroid *a = g->ahead;
		while (a) {
			//Log("draw asteroid...\n");
			glColor3fv(a->color);
			glPushMatrix();
			glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
			glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINE_LOOP);
			//Log("%i verts\n",a->nverts);
			for (int j=0; j<a->nverts; j++) {
				glVertex2f(a->vert[j][0], a->vert[j][1]);
			}
			glEnd();
			//glBegin(GL_LINES);
			//	glVertex2f(0,   0);
			//	glVertex2f(a->radius, 0);
			//glEnd();
			glPopMatrix();
			glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			glVertex2f(a->pos[0], a->pos[1]);
			glEnd();
			a = a->next;
		}
	}
	//-------------------------------------------------------------------------
	//Draw the bullets
	{
		Bullet *b = g->bhead;
		while (b) {
			//Log("draw bullet...\n");
			glColor3f(1.0, 1.0, 1.0);
			glBegin(GL_POINTS);
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
			glEnd();
			b = b->next;
		}
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

void sscreen_background(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//draw textured quad
	//float wid = 120.0f;
	glColor3f(1.0, 0.0, 0.0);
	//glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, bgTexture0);
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
