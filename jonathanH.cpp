//CS335 Zombie movement, collision, and spawner functions
//Jonathan Halbrook
//Instructor: Gordon Griesel
#include <iostream>
#include <cmath>
#include <cstdlib>

extern "C"{
	#include "struct.h"
	#include "defs.h"
}

extern void SpawnZombies(Game *g) 
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
			//flips out occasionally, angle is always wrong at spawn~bware
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
			//angle occasionally wrong at spawn
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
extern void zMove(Game *g, Zombie *a)
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
	//ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
	ta->color[0] = 0.8;
	ta->color[1] = 0.8;
	ta->color[2] = 0.7;
	ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
	ta->vel[1] = a->vel[1] + (rnd()*2.0-1.0);
	//std::cout << "frag" << std::endl;
}
extern void zomb_zomb_collision(Zombie *a)
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
