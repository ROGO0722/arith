#ifndef PNM_INCLUDED
#define PNM_INCLUDED
#include <stdio.h>
#include <stdint.h>

#include <except.h>

#include <a2methods.h>

/*
 * functions in this interface use only the 'new', 'free',
 * 'cell', and 'map_default' methods
 *
 * every function in this interface uses the (x, y) coordinate system,
 * which is the same as the (col, row) or (width, height) system.
 *
 */

/* raised by ppmread when not a pnm file */
extern const Except_T Pnm_Badformat;


/* 
 * Write 'pixmap' to a file.  If 'pixmap' is empty, it's a checked 
 * run-time error. 
 */
void Pnm_ppmwrite(FILE *fp, Pnm_ppm pixmap);

/* Free pixmap pointed to by ppmp. Does not free methods. */
void Pnm_ppmfree(Pnm_ppm *ppmp);

#endif