/**************************************************************
 *
 *                    a2plain.c
 *
 *     Assignment: locality
 *     Authors:  Emily Yuan, 
 *     Date:    2/21/24
 *
 *     Implementation of a2plain. This exports a pointer at the end to the
 *     struct that has the method suite for UArray2.
 *
 **************************************************************/
#include <string.h>
#include "assert.h"
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/********************************************************************
* Function:   new
* Purpose:    creates a new two dimentional array with the specified dimensions
* that are given
* Arguments:     width             num of elems in a column, gives width
*                height            num of elems in a row, gives height
*                size              size of bytes in one elem
*       
* Returns:    a pointer to struct A2Methods_UArray2
* Effects:    allocates memory for a two-dimensional array structure.
              initializes its dimensions and element size. 
*             
* Expects:    valid dimensions for width, height, size
* Notes:      CRE if dimensions are invalid, checked in UArray2 interface
*     
********************************************************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size); 
}

/********************************************************************
* Function:   new_with_blocksize
* Purpose:    creates a new two dimentional array that has blocksize with the 
*             specified dimensions

* Arguments:     width             num of elems in a column, gives width
*                height            num of elems in a row, gives height
*                size              size of bytes in one elem
*                blocksize         num of elements in a block
*       
* Returns:    a pointer to struct A2Methods_UArray2
* Effects:    allocates memory for a two-dimensional array structure.
              initializes its dimensions and element size. 
              blocksize is not used.
*             
* Expects:    valid dimensions for width, height, size
* Notes:      CRE if dimensions are invalid, checked in UArray2 interface
*     
********************************************************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize; 
        return UArray2_new(width, height, size);
}

/********************************************************************
* Function:   free
* Purpose:    frees the memory of the two dimensional array 
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
*       
* Returns:    None
* Effects:    deallocates memory associated with two dimensional arrray. 
*             
* Expects:    valid pointer to uarray2
* Notes:      CRE if uarray2 is not valid 
*     
********************************************************************/
static void a2free(A2Methods_UArray2 *uarray2) 
{
        UArray2_free((UArray2_T *) uarray2);
}

/********************************************************************
* Function:   width
* Purpose:    retrieves the width of the two dimensional UArray
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
*       
* Returns:    an int that gives the width of the uarray2
* Effects:    uses the UArray2_width function to attain the width of the 
              uarray. 
*             
* Expects:    valid pointer to uarray2
* Notes:      CRE if uarray2 is not valid 
*     
********************************************************************/
static int width(A2Methods_UArray2 uarray2)
{
    return UArray2_width(uarray2);
}

/********************************************************************
* Function:   height
* Purpose:    retrieves the height of the two dimensional UArray
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
*       
* Returns:    an int that gives the height of the uarray2
* Effects:    uses the UArray2_height function to attain the height of the 
              uarray. 
*             
* Expects:    valid pointer to uarray2
* Notes:      CRE if uarray2 is not valid 
*     
********************************************************************/
static int height(A2Methods_UArray2 uarray2) 
{
    return UArray2_height(uarray2);
}

/********************************************************************
* Function:   size
* Purpose:    retrieves the size of the two dimensional UArray
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
*       
* Returns:    an int that gives the size of the uarray2
* Effects:    uses the UArray2_height function to retrieved the size of the 
              uarray. 
*             
* Expects:    valid pointer to uarray2
* Notes:      CRE if uarray2 is not valid 
*     
********************************************************************/
static int size(A2Methods_UArray2 uarray2) 
{
    return UArray2_size(uarray2);
}


/********************************************************************
* Function:   blocksize
* Purpose:    retrieves the blocksize of the two dimensional UArray
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
*       
* Returns:    returns 1 because blocksize is not valid here. 
* Effects:    voids uarray2 and essentially this function is not used because
              there is no blocks in UArray2
*             
* Expects:    valid pointer to uarray2
* Notes:      CRE if uarray2 is not valid 
*     
********************************************************************/
static int blocksize(A2Methods_UArray2 uarray2) 
{
    (void) uarray2; 
    return 1; 
}

/********************************************************************
* Function:   at
* Purpose:    retrieves the element at the specificied indices in the Uarray2
*
* Arguments:     A2Methods_UArray2 *uarray2       pointer to the two dimensional
                                                  array in the A2Methods struct
                int i                             col index 
                int j                             row index
*       
* Returns:    element that is at the specificied indices in the UArray2
* Effects:    uses UArray2_at function to retrieve the elem at specified
*             indices
*             
* Expects:    valid pointer to uarray2
*             valid dimensions of i,j 
* Notes:      CRE if uarray2 is not valid 
*             CRE if i and j are not valid dimensions
*     
********************************************************************/
static A2Methods_Object *at(A2Methods_UArray2 uarray2, int i, int j) 
{

    return UArray2_at(uarray2, i, j);
}

/********************************************************************
* Function:   map_row_major
* Purpose:    Map the given function over the elements of the UArray2b_T
*             in row-major order
* Arguments:
*     - UArray2b_T array2b: the UArray2b_T
*     - ApplyFunction apply: the function to apply
*     - void pointer cl: the closure
* Returns:    void
* Effects:    map the given function over the elements of the UArray2b_T
* Expects:    uarray2b is not NULL
*             apply is not NULL
* Notes:      CRE if uarray2b and apply is NULL
********************************************************************/ 
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert(uarray2 != NULL);
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/********************************************************************
* Function:   map_col_major
* Purpose:    Map the given function over the elements of the UArray2b_T
*             in col-major order
* Arguments:
*     - UArray2b_T array2b: the UArray2b_T
*     - ApplyFunction apply: the function to apply
*     - void pointer cl: the closure
* Returns:    
* Effects:    map the given function over the elements of the UArray2b_T
* Expects:    uarray2b is not NULL
*             apply is not NULL
* Notes:      CRE if uarray2b and apply is NULL
********************************************************************/
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert(uarray2);
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};


/********************************************************************
* Function:   apply_small
* Purpose:    run the apply function on the given array
* Arguments:
*     - int i: void
*     - int j: void
*     - UArray2_T uarray2: void
*     - void pointer elem: the element
*     - void pointer vcl: the closure used to apply the function
* Returns:    void
* Effects:    Uses a closure pointer to apply the function to the element
* Expects:    UArray2_T uarray2 is not NULL
* Notes:
********************************************************************/
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        assert(uarray2);
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/********************************************************************
* Function:   small_map_row_major
* Purpose:    Map the given function over the elements of the UArray2b_T
*             in row-major order
* Arguments:
*     - UArray2b_T array2b: the UArray2b_T
*     - ApplyFunction apply: the function to apply
*     - void pointer cl: the closure
* Returns:    void
* Effects:    map the given function over the elements of the UArray2b_T
* Expects:    a2 is not NULL
*             apply is not NULL
* Notes:      CRE if a2, apply, cl are NULL
********************************************************************/ 
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/********************************************************************
* Function:   small_map_col_major
* Purpose:    Map the given function over the elements of the UArray2b_T
*             in col-major order
* Arguments:
*     - UArray2b_T array2b: the UArray2b_T
*     - ApplyFunction apply: the function to apply
*     - void pointer cl: the closure
* Returns:    void
* Effects:    map the given function over the elements of the UArray2b_T
* Expects:    a2 is not NULL
*             apply is not NULL
* Notes:      CRE if a2, apply, cl are NULL
********************************************************************/
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,  
        height, 
        size,   
        blocksize, 
        at,
        map_row_major,
        map_col_major,
        NULL,
        map_row_major,
        small_map_row_major,
        small_map_col_major,
        NULL, 
        small_map_row_major,
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
