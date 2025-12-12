#ifndef COMMON_H
#define COMMON_H

#define RIGHT_SIDE 0
#define LEFT_SIDE 1

typedef bool direction_t;

#ifndef MAX
#define MAX(a, b) ((a > b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a < b) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(val, upper, lower) (MAX(MIN(val, upper), lower))
#endif

#endif
