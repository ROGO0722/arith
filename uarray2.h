#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#include "uarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mem.h>

#define T UArray2_T
typedef struct T *T;

struct T {
        int width, height;
        int size; 
        UArray_T data;
};

typedef void UArray2_applyfun(int i, int j, T array2, void *elem, void *cl); 
typedef void UArray2_mapfun(T array2, UArray2_applyfun apply, void *cl);
extern T     UArray2_new   (int width, int height, int size);
extern void  UArray2_free  (T *array2);
extern int   UArray2_width (T array2);
extern int   UArray2_height(T array2);
extern int   UArray2_size  (T array2);
extern void *UArray2_at    (T array2, int i, int j);
extern void  UArray2_map_row_major(T array2, UArray2_applyfun apply, void *cl);
extern void  UArray2_map_col_major(T array2, UArray2_applyfun apply, void *cl);

#undef T
#endif