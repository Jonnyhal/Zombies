//sound functions
#include <iostream>

#define USE_SOUND
#ifndef USE_SOUND 
#include <FMOD/wncompat.h>
#include "fmod.h"
#endif
#include "sounds.h"
using namespace std;

extern void init_sounds(void)
{
	#ifdef USE_SOUND
	//FMOD_RESULT result;
	if (fmod_init()) {
		cout << "ERROR - fmod_init()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/tick.wav", 0)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/Hit_Hurt.wav", 1)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/Laser_Shoot.wav", 2)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	//if ((fmod_createsound((char *)"./sounds/
	mod_setmode(0,FMOD_LOOP_OFF);
	//fmod_playsound(0);
	//fmod_systemupdate();
	#endif //USE_SOUND
}
