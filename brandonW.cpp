// Brandon Ware
// 5/9/2015
// brandonW.cpp
//------------------
// Houses bullet functions such as
// Creation, Position updating, drawing to screen
// and collision
//------------------
// Houses loot drop function + timers for
// loot drops & multi
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <string>
#include <X11/keysym.h>
extern "C"{
	#include "struct.h"
	#include "fonts.h"
}
#include "delete.h"
#include "loot.h"
#include "bullets.h"
#include "other.h"
#include "fmod.h"
extern void fire_weapon(Game *g)
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
				fmod_playsound(5);
				b->type = 1;
				g->player1.oldbType = 1;
				timeCopy(&b->time, &bt);
				b->pos[0] = g->player1.pos[0];
				b->pos[1] = g->player1.pos[1];
				b->origin[0] = g->player1.pos[0];
				b->origin[1] = g->player1.pos[1];
				b->origin[2] = g->player1.pos[2];
				b->vel[0] = 0;
				b->vel[1] = 0;
				b->angle = g->player1.angle+90.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
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
				fmod_playsound(2);
				g->player1.oldbType = 2;
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
				b->angle = g->player1.angle+110.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
				c->angle = g->player1.angle+70.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
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
				fmod_playsound(2);
				g->player1.oldbType = 3;
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
				b->angle = g->player1.angle+110.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
				c->angle = g->player1.angle+70.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
				d->angle = g->player1.angle+90.0;
				if(b->angle > 360)
					b->angle = b->angle - 360;
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

extern void bulletDraw(Bullet *b)
{
	while (b) {
		//Log("draw bullet...\n");
		//glColor3f(1.0, 1.0, 1.0);
		glColor3fv(b->color);
		glPushMatrix();
		//std::cout<<"bullet angle: " << b->angle << "\n";
		//std::cout<<"bullet posxy: " << b->pos[0] << ", " << b->pos[1] << ", " << b->pos[2] << "\n";
		glTranslatef(b->pos[0], b->pos[1], 0.0f);
                if (b->type == 1) {
                        glRotatef(b->angle, 0.0f, 0.0f, 1.0f);
                } else if (b->type == 2) {

                        glRotatef(b->angle, 0.0f, 0.0f, 1.0f);
                } else if (b->type == 3) {
                        glRotatef(b->angle, 0.0f, 0.0f, 1.0f);
                }

		glBegin(GL_QUADS);
			glVertex2f(-8.0f, 2.0f);
			glVertex2f(8.0f, 2.0f);
			glVertex2f(8.0f, -2.0f);
			glVertex2f(-8.0f, -2.0f);
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

extern void bulletAng(Bullet *b)
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

extern void updateBulletPos(Game *g, Bullet *b)
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
		//delete old bullets if new bullet type is gained
		if (g->player1.check2 == 1) {
                        Bullet *saveb = b->next;
                        deleteBullet(g, b);
                        b = saveb;
                        g->nbullets--;
                        continue;
                }

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

extern void bul_zomb_collision(Game *g, Bullet *x)
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
					//fmod_playsound(8);
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
					//fmod_playsound(7);
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

extern void player_loot_collision(Game *g, Loot *l)
{
	int d0, d1, dist;
	d0 = g->player1.pos[0] - l->pos[0];
	d1 = g->player1.pos[1] - l->pos[1];
	dist = (d0*d0 + d1*d1);
	if (dist < 200) {
		//loot is hit
		std::cout<<"loot collision!\n";
		powerUp(g, l);
		deleteLoot(g, l);
	}
}

extern void powerUp(Game *g, Loot *l)
{
	switch (l->type) {
		case 0:
			//nothing
			break;
		case 1:
			//rapid fire
			g->player1.tempRF = 1;
			rftime(g);
			break;
		case 2:
			//double shot
			g->player1.bulletType = 2;
			fmod_playsound(4);
			break;
		case 3: 
			//triple shot
			g->player1.bulletType = 3;
			fmod_playsound(4);
			break;
		case 4:
			//temp invuln
			g->player1.tempinvuln = 1;
			g->player1.invuln = 1;
			invulntime(g);
			break;
		case 5:
			//wave clear
			break;
		case 6:
			//1up
			g->player1.lives++;
			break;
	}
}
/*=============== LOOT IDEAS ==================
  - 5% chance for a drop to occur...
  ------------------------------------
  - 25% chance to drop double shot
  - 12% chance to drop triple shot
  -  5% chance to drop extra life
  - 40% chance to drop rapid fire
  -  8% chance to drop screen clear(won't work on boss)
  - 10% chance to drop 10s invulnerability
  =============================================*/
extern void lootDrop(Game *g, Zombie *a)
{
	//to prevent deprecation errors
	char texname[] = "./images/doubleshot.ppm";
	char texname1[] = "./images/tripleshot.ppm";
	char texname2[] = "./images/nuke.ppm";
	char texname3[] = "./images/lifeup.ppm";
	char texname4[] = "./images/rapidfire.ppm";
	char texname5[] = "./images/invuln.ppm";
	//Load image files
	// Create a random number from 1-100
	int r1 = rand() % 100 + 1;
	std::cout<<r1<<"\n";
	if (r1%20 == 0) {
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
			g->lhead->lootbg = ppm6GetImage(texname4);
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
				g->lhead->lootbg = ppm6GetImage(texname);
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
				g->lhead->lootbg = ppm6GetImage(texname1);
				glGenTextures(1, &g->lhead->lootTex);
				init_textures(g->lhead->lootbg, g->lhead->lootTex);
			}
		} else if (r2 >=78 && r2 <= 87) {
			//temp invuln!
			g->lhead->type = 4;
			std::cout << "INVULNERABILITY DROP\n";
			g->lhead->lootbg = ppm6GetImage(texname5);
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		} else if (r2 >=88 && r2 <= 95) {
			//free wave clear!
			g->lhead->type = 5;
			std::cout << "WAVE CLEAR DROP\n";
			g->lhead->lootbg = ppm6GetImage(texname2);
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		} else {
			//1up!!!
			g->lhead->type = 6;
			std::cout << "LIFE UP DROP\n";
			g->lhead->lootbg = ppm6GetImage(texname3);
			glGenTextures(1, &g->lhead->lootTex);
			init_textures(g->lhead->lootbg, g->lhead->lootTex);
		}
	}
}

extern void lootTime(Loot *l)
{
	struct timespec lt;
	clock_gettime(CLOCK_REALTIME, &lt);
	double ts = timeDiff(&l->lootTimer, &lt);
	if (ts > 0.1) {
		timeCopy(&l->lootTimer, &lt);
	}
}

extern void lootDraw(GLuint tex, Loot *l, float r, float g, float b, float alph)
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


/*==========================================================================================
  ==========================================================================================
					KEY STUFF
  ==========================================================================================
  ==========================================================================================*/
extern std::string keyCheck(Game *g)
{
	int shift = 0;
	int count = 0;
	std::string name = "";
	if (keys[XK_Shift_L] || keys[XK_Shift_R])
		shift = 1;
	if (keys[XK_Caps_Lock])
		shift = 1;

	if (keys[XK_q]) {
		if (shift) {
			name += "Q";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		} else {
			name += "q";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_q] = 0;
	}
	if (keys[XK_w]) {
		if (shift) {
			name += "W";
			g->oldspacing = g->spacing;
			g->spacing+=22;
		} else {
			name += "w";
			g->oldspacing = g->spacing;
			g->spacing+=16;
		}
		count++;
		keys[XK_w] = 0;
	}
	if (keys[XK_e]) {
		if (shift) {
			name += "E";
			g->oldspacing = g->spacing;
			g->spacing+=12;
		} else {
			name += "e";
			g->oldspacing = g->spacing;
			g->spacing+=11;
		}
		count++;
		keys[XK_e] = 0;
	}
	if (keys[XK_r]) {
		if (shift) {
			name += "R";
			g->oldspacing = g->spacing;
			g->spacing+=12;
		} else {
			name += "r";
			g->oldspacing = g->spacing;
			g->spacing+=7;
		}
		count++;
		keys[XK_r] = 0;
	}
	if (keys[XK_t]) {
		if (shift) {
			name += "T";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "t";
			g->oldspacing = g->spacing;
			g->spacing+=6;
		}
		count++;
		keys[XK_t] = 0;
	}
	if (keys[XK_y]) {
		if (shift) {
			name += "Y";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		} else {
			name += "y";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_y] = 0;
	}
	if (keys[XK_u]) {
		if (shift) {
			name += "U";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "u";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_u] = 0;
	}
	if (keys[XK_i]) {
		if (shift) {
			name += "I";
			g->oldspacing = g->spacing;
			g->spacing+=3;
		}else {
			name += "i";
			g->oldspacing = g->spacing;
			g->spacing+=3;
		}
		count++;
		keys[XK_i] = 0;
	}
	if (keys[XK_o]) {
		if (shift) {
			name += "O";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		} else {
			name += "o";
			g->oldspacing = g->spacing;
			g->spacing+=11;
		}
		count++;
		keys[XK_o] = 0;
	}
	if (keys[XK_p]) {
		if (shift) {
			name += "P";
			g->oldspacing = g->spacing;
			g->spacing+=12;
		} else {
			name += "p";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_p] = 0;
	}
	if (keys[XK_a]) {
		if (shift) {
			name += "A";
			g->oldspacing = g->spacing;
			g->spacing+=14;
		} else {
			name += "a";
			g->oldspacing = g->spacing;
			g->spacing+=11;
		}
		count++;
		keys[XK_a] = 0;
	}
	if (keys[XK_s]) {
		if (shift) {
			name += "S";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "s";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_s] = 0;
	}
	if (keys[XK_d]) {
		if (shift) {
			name += "D";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "d";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_d] = 0;
	}
	if (keys[XK_f]) {
		if (shift) {
			name += "F";
			g->oldspacing = g->spacing;
			g->spacing+=11;
		} else {
			name += "f";
			g->oldspacing = g->spacing;
			g->spacing+=8;
		}
		count++;
		keys[XK_f] = 0;
	}
	if (keys[XK_g]) {
		if (shift) {
			name += "G";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		} else {
			name += "g";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_g] = 0;
	}
	if (keys[XK_h]) {
		if (shift) {
			name += "H";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "h";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_h] = 0;
	}
	if (keys[XK_j]) {
		if (shift) {
			name += "J";
			g->oldspacing = g->spacing;
			g->spacing+=9;
		} else {
			name += "j";
			g->oldspacing = g->spacing;
			g->spacing+=5;
		}
		count++;
		keys[XK_j] = 0;
	}
	if (keys[XK_k]) {
		if (shift) {
			name += "K";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "k";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_k] = 0;
	}
	if (keys[XK_l]) {
		if (shift) {
			name += "L";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		} else {
			name += "l";
			g->oldspacing = g->spacing;
			g->spacing+=3;
		}
		count++;
		keys[XK_l] = 0;
	}
	if (keys[XK_z]) {
		if (shift) {
			name += "Z";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "z";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_z] = 0;
	}
	if (keys[XK_x]) {
		if (shift) {
			name += "X";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		} else {
			name += "x";
			g->oldspacing = g->spacing;
			g->spacing+=11;
		}
		count++;
		keys[XK_x] = 0;
	}
	if (keys[XK_c]) {
		if (shift) {
			name += "C";
			g->oldspacing = g->spacing;
			g->spacing+=14;
		} else {
			name += "c";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_c] = 0;
	}
	if (keys[XK_v]) {
		if (shift) {
			name += "V";
			g->oldspacing = g->spacing;
			g->spacing+=14;
		} else {
			name += "v";
			g->oldspacing = g->spacing;
			g->spacing+=12;
		}
		count++;
		keys[XK_v] = 0;
	}
	if (keys[XK_b]) {
		if (shift) {
			name += "B";
			g->oldspacing = g->spacing;
			g->spacing+=12;
		} else {
			name += "b";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_b] = 0;
	}
	if (keys[XK_n]) {
		if (shift) {
			name += "N";
			g->oldspacing = g->spacing;
			g->spacing+=13;
		} else {
			name += "n";
			g->oldspacing = g->spacing;
			g->spacing+=10;
		}
		count++;
		keys[XK_n] = 0;
	}
	if (keys[XK_m]) {
		if (shift) {
			name += "M";
			g->oldspacing = g->spacing;
			g->spacing+=16;
		} else {
			name += "m";
			g->oldspacing = g->spacing;
			g->spacing+=15;
		}
		count++;
		keys[XK_m] = 0;
	}
	if (keys[XK_space]) {
		name += " ";
		g->oldspacing = g->spacing;
		g->spacing+=7;
		count++;
		keys[XK_space] = 0;
	}
	/*if (keys[XK_BackSpace] || keys[XK_Delete]) {
		name = "^";
		g->spacing -= (g->spacing - g->oldspacing);
		keys[XK_BackSpace] = 0;
		keys[XK_Delete] = 0;
	}*/
	return name;
}






/*=====================================TIMERS====================================*/
extern void multitime(Game *g)
{
	struct timespec mt;
	clock_gettime(CLOCK_REALTIME, &mt);
	double ts = timeDiff(&g->player1.multiTimer, &mt);
	if (ts > 0.1) {
		timeCopy(&g->multiTimer, &mt);
		timeCopy(&g->player1.multiTimer, &mt);
	}
}

extern void updateMulti(Game *g)
{
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->player1.multiTimer, &bt);
	if (ts > 2) {
		//time to reset timer.
		g->player1.multi = 1.0;
	}
}

extern void invulntime(Game *g)
{
	struct timespec mt;
	clock_gettime(CLOCK_REALTIME, &mt);
	double ts = timeDiff(&g->player1.invulnTimer, &mt);
	if (ts > 0.1) {
		timeCopy(&g->player1.invulnTimer, &mt);
	}
}

extern void updateInvuln(Game *g)
{
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->player1.invulnTimer, &bt);
	if (ts > 10) {
		//turn off invuln
		g->player1.tempinvuln = 0;
		g->player1.invuln = 0;
	}
}

extern void rftime(Game *g)
{
	struct timespec mt;
	clock_gettime(CLOCK_REALTIME, &mt);
	double ts = timeDiff(&g->player1.rfTimer, &mt);
	if (ts > 0.1) {
		timeCopy(&g->player1.rfTimer, &mt);
	}
}

extern void updateRF(Game *g)
{
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g->player1.rfTimer, &bt);
	if (ts > 10) {
		//time to reset timer.
		g->player1.tempRF = 0;
	}
}




