#include <stdint.h>

#include <except.h>
#include <a2methods.h>


/* colored pixel (scaled integers) */
typedef struct rgb_float {
        unsigned red, green, blue;
} *rgb_float;

/*
 * clients may read 'methods', but to mutate 'methods' or any 
 * field of 'methods' is an UNCHECKED run-time error
 */
typedef struct Pnm_float {
        unsigned width, height, denominator;
        A2Methods_UArray2 pixels;          /* 2D array with
                                            * element type 'struct Pnm_rgb' 
                                            */
        const struct A2Methods_T *methods; /* to operate on 'pixels' field */
} *Pnm_float;