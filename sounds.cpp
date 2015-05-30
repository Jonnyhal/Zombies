//sound functions
#include <iostream>
#include "sounds.h"
#define USE_SOUND
#include <FMOD/fmod.h>
#include <FMOD/wincompat.h>
#include "fmod.h"

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
	if (fmod_createsound((char *)"./sounds/gun3.wav", 2)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/chainsaw_hedgemaze.mp3", 3)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/reload.wav", 4)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/m1_single.wav", 5)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/zomb1.wav", 6)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/impact.mp3", 7)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/zombmoan.wav", 8)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	if (fmod_createsound((char *)"./sounds/gun2.wav", 9)) {
		cout << "ERROR - fmod_createsound()\n" << endl;
		return;
	}
	//if ((fmod_createsound((char *)"./sounds/
	fmod_setmode(0,FMOD_LOOP_OFF);
	fmod_setmode(3,FMOD_LOOP_NORMAL);
	//fmod_playsound(0);
	//fmod_systemupdate();
	#endif //USE_SOUND
}
