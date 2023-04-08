#ifndef __BC23_DEF
#define __BC23_DEF

#define ASPECT_RATIO    (256.0/224.0)
#define SCALE           2
#define BASE_HEIGHT     224
#define HEIGHT          (BASE_HEIGHT*SCALE)
#define BASE_WIDTH      ((int)((float)BASE_HEIGHT*ASPECT_RATIO))
#define WIDTH           (BASE_WIDTH*SCALE)

#endif