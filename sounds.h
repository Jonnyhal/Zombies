#ifndef _SOUNDS_H_
#define _SOUNDS_H_
//sound.h prototypes for sound functions
#include <iostream>
#include <X11/Xlib.h>
#include "log.h"
#include "ppm.h"
extern "C" {
	#include "fonts.h"
}
#define USE_SOUND
#ifndef USE_SOUND
#include <FMOD/fmod.h>
#include <FMOD/wincompat.h>
#include "fmod.h"
#endif

extern void init_sounds(void);

#endif
