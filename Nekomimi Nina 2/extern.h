// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_GITHUB_REPO


#ifdef N_GITHUB_REPO


#import <Cocoa/Cocoa.h>


#include "../nonnon/mac/_mac.c"
#include "../nonnon/mac/draw.c"
#include "../nonnon/mac/image.c"
#include "../nonnon/mac/sound.c"
#include "../nonnon/mac/window.c"

#include "../nonnon/mac/gamepad.c"


#include "../nonnon/neutral/time.c"

#include "../nonnon/neutral/bmp/ui/rectframe.c"
#include "../nonnon/neutral/bmp/ui/roundframe.c"

#include "../nonnon/neutral/bmp/ui/progressbar.c"
#include "../nonnon/neutral/bmp/ui/timer.c"
#include "../nonnon/neutral/bmp/ui/transition.c"


#include "../nonnon/bridge/gdi.c"


#else  // #ifdef N_GITHUB_REPO


#import <Cocoa/Cocoa.h>


#include "../../nonnon/mac/_mac.c"
#include "../../nonnon/mac/draw.c"
#include "../../nonnon/mac/image.c"
#include "../../nonnon/mac/sound.c"
#include "../../nonnon/mac/window.c"

#include "../../nonnon/mac/gamepad.c"


#include "../../nonnon/neutral/time.c"

#include "../../nonnon/neutral/bmp/ui/rectframe.c"
#include "../../nonnon/neutral/bmp/ui/roundframe.c"

#include "../../nonnon/neutral/bmp/ui/progressbar.c"
#include "../../nonnon/neutral/bmp/ui/timer.c"
#include "../../nonnon/neutral/bmp/ui/transition.c"


#include "../../nonnon/bridge/gdi.c"


#endif // #ifdef N_GITHUB_REPO

