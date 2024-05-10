#include "util.h"

u16 btohw(u8 lower, u8 upper) { return (upper << 8) | lower; }
