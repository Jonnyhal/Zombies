//Alvaro Juban Jr.
//CS 335 Texture Rendering
//Instructor: Gordon Griesel
//-------------------------
// Renders the objects 
// then applies Textures 
// then draws to the screen.

#include <iostream>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/gl.h>
extern "C"{
	#include "struct.h"
	#include "fonts.h"
}
#include "render.h"
#include "bullets.h"
#include "loot.h"
#include "other.h"
#include "delete.h"
int bur = 0;


extern void render(Game *g)
{
//GLuint silhouetteTexture;
//GLuint silhouette_player_Texture;
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
		//std::cout<<"loot timer: " << ts << "\n";
                if (ts > 3) {
                        //MAKE IT BLINK!!!!!!!!
                        if (ts < 3.15)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                        else if(ts > 3.30 && ts < 3.45)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                        else if(ts > 3.60 && ts < 3.75)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                        else if(ts > 3.90 && ts < 4.05)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                        else if(ts > 4.20 && ts < 4.35)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                        else if(ts > 4.50 && ts < 4.65)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
 			else if(ts > 4.80 && ts < 5.00)
                                lootDraw(l->lootTex, l ,1.0, 1.0, 1.0, 1.0);
                 	else if (ts > 5.00 || l->type == 0) {
                        	//time to delete loot
                        	deleteLoot(g,l);
				std::cout<<"deleteloot() is called\n";
			}
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
        ggprint8b(&r, 16, 0x00ff1111, "Lives: %i", g->player1.lives);
	//-------------------------------------------------------------------------
	//Draw the crosshair================
	//glColor3fv(g->cross->color);
	glPushMatrix();
	glTranslatef(g->cross->pos[0], g->cross->pos[1], g->cross->pos[2]);	
        glBindTexture(GL_TEXTURE_2D, silhouette_mouse_Texture);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
	
	glColor4ub(255,255,255,255);

	glBegin(GL_QUADS);
		float m = 27.0f;
                glTexCoord2f(0.0f, 0.0f); glVertex2f(-m,  m);
                glTexCoord2f(1.0f, 0.0f); glVertex2f( m,  m);
                glTexCoord2f(1.0f, 1.0f); glVertex2f( m, -m);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(-m, -m);

	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
	//===================================
	//Draw the player1
        glColor3fv(g->player1.color);
        glPushMatrix();
        glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
        //float angle = atan2(player1.dir[1], player1.dir[0]);
        //std::cout<<"angle = " << g->player1.angle << std::endl;
        
	if ( g->player1.bulletType == 2) {
        glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture_2);
        }
        else if ( g->player1.bulletType == 3) {
        glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture_3);
        }
        else {
        glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture);
        }	
	
        glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4ub(255,255,255,255);
        glBegin(GL_QUADS);
                //float w = g->player1.width;
                float w = 28.0f;
                glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
                glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
                glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
        glEnd();
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        //glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
        //glVertex2f(-10.0f, -10.0f);
        //glVertex2f(  0.0f, 20.0f);
        //glVertex2f( 10.0f, -10.0f);
        //glColor3f(1.0f, 0.0f, 0.0f);
        //glBegin(GL_POINTS);
        //glVertex2f(0.0f, 0.0f);
        //glEnd();
        //glPopMatrix();
        if (g->player1.invuln) {
                float col[3] = {0.0, 0.0, 0.0};
                glColor3fv(col);
                glPushMatrix();
		glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
        	//float angle = atan2(player1.dir[1], player1.dir[0]);
	        //std::cout<<"angle = " << g->player1.angle << std::endl;
	        
		if ( g->player1.bulletType == 2) {
	        	glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture_2);
	        }
	        else if ( g->player1.bulletType == 3) {
	        	glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture_3);
	        }
	        else {
	        	glBindTexture(GL_TEXTURE_2D, silhouette_player_Texture);
	        }	
		
	        glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
	        glEnable(GL_ALPHA_TEST);
	        glAlphaFunc(GL_GREATER, 0.0f);
	        glColor4ub(0,0,0,255);
	        glBegin(GL_QUADS);
                //float w = g->player1.width;
	                float w = 28.0f;
	                glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
	                glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
	                glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
	                glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
	        glEnd();
	        glPopMatrix();
	        glBindTexture(GL_TEXTURE_2D, 0);
	        glDisable(GL_ALPHA_TEST);

                /*glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
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
                glPopMatrix();*/
                //std::cout<< "blinking!" << "\n";
        }
		//========= animation ==========//	
	        if (keys[XK_w] || keys[XK_a] || keys[XK_d] || keys [XK_s]) {
                if(bur < 11 || bur > 21)
                {
                //Draw the player1
                glColor3fv(g->player1.color);
                glPushMatrix();
                glTranslatef(g->player1.pos[0], g->player1.pos[1], g->player1.pos[2]);
                //float angle = atan2(player1.dir[1], player1.dir[0]);
                //std::cout<<"angle = " << g->player1.angle << std::endl;

                        if (bur < 11) {
                                glBindTexture(GL_TEXTURE_2D, RFsilhouette_player_Texture);
                                bur++;
                                }
                        else {
                                glBindTexture(GL_TEXTURE_2D, LFsilhouette_player_Texture);
                                bur++;
                                }
                        if (bur == 31) { bur = 0; };

                        glRotatef(g->player1.angle, 0.0f, 0.0f, 1.0f);
                        glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f);
                        glColor4ub(255,255,255,255);
                        glBegin(GL_QUADS);
                                //float w = g->player1.width;
                                float w = 28.0f;
                                glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
                                glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
                                glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
                                glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
                        glEnd();
                        glPopMatrix();
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glDisable(GL_ALPHA_TEST);
                }
                else
                bur++;
	}		
        //-------------------------------------------------------------------------
        //Draw the zombies
        {
                Zombie *a = g->ahead;
                int count = 1;
                while (a) {
                        //Log("draw asteroid...\n");
			glColor3fv(g->player1.color);
                        glPushMatrix();
                        glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
			if( rzomb == 0) { 
				glBindTexture(GL_TEXTURE_2D, BsilhouetteTexture);
			}
			else if( rzomb == 1) { 
				glBindTexture(GL_TEXTURE_2D, NsilhouetteTexture);
			}
			else { 
				glBindTexture(GL_TEXTURE_2D, silhouetteTexture);
			}
			glRotatef(a->angle, 0.0f, 0.0f, 1.0f);
                        glEnable(GL_ALPHA_TEST);
                        glAlphaFunc(GL_GREATER, 0.0f);
                        glBegin(GL_QUADS);
                                // float w is zombie size
                                float w = 27.0f;
                                glTexCoord2f(0.0f, 0.0f); glVertex2f(-w,  w);
                                glTexCoord2f(1.0f, 0.0f); glVertex2f( w,  w);
                                glTexCoord2f(1.0f, 1.0f); glVertex2f( w, -w);
                                glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, -w);
                        glEnd();
                        glPopMatrix();
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glDisable(GL_ALPHA_TEST);
                        glColor3f(1.0f, 0.0f, 0.0f);
                        glBegin(GL_POINTS);
                        glVertex2f(a->pos[0], a->pos[1]);
                        glEnd();
                        count++;
                        a = a->next;
                }
        }
        //std::cout<<"player angle: " << g->player1.angle << "\n";
        //-------------------------------------------------------------------------
        //Draw the bullets
                //std::cout<<"player posxy: " << g->player1.pos[0] << ", " << g->player1.pos[1] << ", " << g->player1.pos[2] << "\n";
        bulletDraw(g->bhead);
        bulletDraw(g->chead);
        bulletDraw(g->dhead);
        //Draw Loot
        //sscreen_background(g->zhead->zTexture, 1.0, 1.0, 1.0, 1.0);
}

