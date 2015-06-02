//CS335 Zombie movement, collision, and spawner functions
//Jonathan Halbrook
//Instructor: Gordon Griesel
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <X11/Xlib.h>
#include <X11/keysym.h>
extern "C"{
	#include "struct.h"
	#include "fonts.h"
}
#include "other.h"
#include "delete.h"
#include "fmod.h"

extern void init(Game *g) {
	std::cout<<"zcnt: " << g->zcnt;
        std::cout<<" wcnt: " << g->wcnt;

        if (g->wcnt > 3 && (g->zcnt%4) == 1) {
                Zone *z = new Zone;
                z->wave = new Wave;
                deleteZone(g,g->zhead);
                if (g->zombieSpawn < 100)
                    g->zombieSpawn *= 2;
                g->zhead = z;
                g->zcnt++;
                g->wcnt = 1;
                char texname[] = "./images/map2.ppm";
                g->zhead->zbackground = ppm6GetImage(texname);
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
        } else if (g->wcnt > 3 && (g->zcnt%4) == 2) {
                Zone *z = new Zone;
                z->wave = new Wave;
                deleteZone(g,g->zhead);
                if (g->zombieSpawn < 100)
                        g->zombieSpawn *= 2;
                g->zhead = z;
                g->zcnt++;
                g->wcnt = 1;
                char texname[] = "./images/map3.ppm";
                g->zhead->zbackground = ppm6GetImage(texname);
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

        } else if (g->wcnt > 3 && (g->zcnt%4) == 3) {
                Zone *z = new Zone;
                z->wave = new Wave;
                deleteZone(g,g->zhead);
                if (g->zombieSpawn < 100)
                        g->zombieSpawn *= 2;
                g->zhead = z;
                g->zcnt++;
                g->wcnt = 1;
                char texname[] = "./images/map4.ppm";
                g->zhead->zbackground = ppm6GetImage(texname);
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
        } else if (g->wcnt > 3 && (g->zcnt%4) == 0) {
                Zone *z = new Zone;
                z->wave = new Wave;
                deleteZone(g,g->zhead);
                if (g->zombieSpawn < 100)
                        g->zombieSpawn *= 2;
                g->zhead = z;
                g->zcnt++;
                g->wcnt = 1;
                char texname[] = "./images/tex3check.ppm";
                g->zhead->zbackground = ppm6GetImage(texname);
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
                char texname[] = "./images/map1.ppm";
                g->zhead->zbackground = ppm6GetImage(texname);
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

extern void spawnZombies(Game *g)
{
	rzomb = rand()%3;
        std::cout<<"\nwcnt in spawnZombies: " << g->wcnt << "\n";
        std::cout<<"zcnt in spawnZombies: " << g->zcnt << "\n";
        int cnt;
        for (int j=0; j<g->zombieSpawn; j++) {
                Zombie *a = new Zombie;
                a->nverts = 4;
                a->radius = 40.0;
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
                cnt = (j % 4);
                if (cnt == 0) {
                        //left
                        a->pos[0] = (Flt)(0);
                        a->pos[1] = (Flt)(rnd() * yres);
                        a->pos[2] = 0.0f;
                        a->color[0] = 0.5;
                        a->color[1] = 2.5;
                        a->color[2] = 1.0;
                } else if (cnt == 1) {
                        //right
                        a->pos[0] = (Flt)(xres);
                        a->pos[1] = (Flt)(rnd() * yres);
                        a->pos[2] = 0.0f;
                        a->color[0] = 0.5;
                        a->color[1] = 2.5;
                        a->color[2] = 1.0;
                } else if (cnt == 2) {
                        //top
                        a->pos[0] = (Flt)(rnd() * xres);//(xres*rnd())
                        a->pos[1] = (Flt)(yres);
                        a->pos[2] = 0.0f;
                        a->color[0] = 0.5;
                        a->color[1] = 2.5;
                        a->color[2] = 1.0;
                } else if (cnt == 3) {
                        //bottom
                        a->pos[0] = (Flt)(rnd() * xres);
                        a->pos[1] = (Flt)(0);
                        a->pos[2] = 0.0f;
                        a->color[0] = 0.5;
                        a->color[1] = 2.5;
                        a->color[2] = 1.0;
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
        clock_gettime(CLOCK_REALTIME, &g->player1.invulnTimer);
        clock_gettime(CLOCK_REALTIME, &g->player1.rfTimer);
        g->player1.radius = 10;

}
//=========Zombie movement Function====
extern void zMove(Game *g, Zombie *a)
{
        Flt d0, d1, dist;
        float x0, y0, x1, y1;
        //zombie to player movement
        d0 = g->player1.pos[0] - a->pos[0];
        d1 = g->player1.pos[1] - a->pos[1];
        dist = sqrt(d0*d0 + d1*d1);
        if (dist < 100000) {
                a->vel[0] = d0/dist * a->speed;
                a->vel[1] = d1/dist * a->speed;
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

extern void buildZombieFragment(Zombie *ta, Zombie *a)
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

extern void zomb_zomb_collision(Zombie *a)
{
        Zombie *c = a->next;
        Flt z0, z1, zdist;
        //zombie on zombie collision
        while (c) {
                z0 = a->pos[0] - c->pos[0];
                z1 = a->pos[1] - c->pos[1];
                zdist = sqrt(z0*z0 + z1*z1);
                if(zdist <= a->radius && zdist != 0) {
                        a->vel[0] += z0/zdist * 5.0;
                        a->vel[1] += z1/zdist * 5.0;
                }
                c = c->next;
        }

}

