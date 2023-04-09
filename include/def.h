#ifndef __BC23_DEF
#define __BC23_DEF

#define ASPECT_RATIO    (256.0/224.0)
#define SCALE           2
#define BASE_HEIGHT     224
#define HEIGHT          (BASE_HEIGHT*SCALE)
#define BASE_WIDTH      ((int)((float)BASE_HEIGHT*ASPECT_RATIO))
#define WIDTH           (BASE_WIDTH*SCALE)

#define TEXT_PINK   "\xff\xff\xa8\xd1"
#define TEXT_WHITE  "\xff\xff\xff\xff"
#define TEXT_RED    "\xff\xff\x61\x61"
#define TEXT_CYAN   "\xff\xc7\xf9\xff"
#define TEXT_ORANGE "\xff\xff\xa2\x69"

#endif