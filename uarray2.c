/**************************************************************
 *
 *                    uarray2.c
 *
 *     Assignment: locality
 *     Authors:  Emily Yuan, Roberto Heo
 *     Date:    2/21/24
 *
 *     Implementation of uarray2. We are setting up an implementation in which
 *     the elements of our two-dimensional array are in one-to-one 
 *     correspondence with elements in a one dimensional UArray T. 
 **************************************************************/
#include "uarray2.h"


#define T UArray2_T

static inline UArray_T row(T a, int j);
static int is_ok(T a);


/********** UArray2_new ********
 *
 * Takes in an integer width and height as dimensions for the UArray2,
 * and an integer size in bytes of the elements that will be stored.
 * Constructs and returns a two dimensional array to the specifications
 * of the parameters.
 *
 * Parameters:
 *      int width		Number of columns for the new array
 *      int height		Number of rows for the new array
 *      int size		Size in bytes of each element to be 
 *                                              stored in the array.
 *
 * Return: Returns a pointer to the new UArray2
 *
 * Expects:
 *      Width and height parameters must both be greater than 0 to be valid 
 *      Width and height parameter can be 0 x 0 
 *      Size must be greater than 0 for the elements that will be stored

 * Notes:
 *      May CRE if malloc for UArray2 fails
 *      CRE if width, height, size are not valid parameters (if not > 0)

 ************************/
T UArray2_new(int width, int height, int size)
{
        int i;
        assert((width == 0 && height == 0 ) || (width > 0 && height > 0));
        assert(size > 0); 
        T arr;
        NEW(arr);
        assert(arr);
        arr->width = width;
        arr->height = height;
        arr->size = size;
        arr->data = UArray_new(height, sizeof(UArray_T));
        for (i = 0; i < height; i++) {
                UArray_T *row = UArray_at(arr->data, i);
                *row = UArray_new(width, size);
        }
        assert(is_ok(arr));
        return arr;
}

/********** UArray2_free ********
 *
 * Takes in the address of a pointer to a UArray2. Frees memory associated with
 * the UArray2 whose pointer’s address is specified by the first parameter. 
 *
 * Parameters:
 *      T *uarray2 	Address of a pointer to a UArray2 to be measured
 *
 * Return: None
 *
 * Expects:
 *      The address of pointer to the array (T *) must not be null
 *      The pointer to the array (T) must not be null
 *
 * Notes:
 *      CRE if address of the pointer to the UArray2 is null, 
 *      or if the pointer to the UArray2 is null

 ************************/
void UArray2_free(T *uarray2)
{
        int i;
        assert(uarray2 && *uarray2);
        for (i = 0; i < (*uarray2)->height; i++) {
                UArray_T p = row(*uarray2, i);
                UArray_free(&p);
        }
        UArray_free(&(*uarray2)->data);
        FREE(*uarray2); 
        *uarray2 = NULL;
}

/********** UArray2_width ********
 *
 * Takes in a pointer to a UArray2, and returns an integer equal to the 
 * width (number of columns) of the two dimensional array.
 *
 * Parameters:
 *      T uarray2		Pointer to a UArray2 to be measured
 *
 * Return: An integer equivalent to the width of the two dimensional array
 *
 * Expects:
 *      The pointer to the UArray2 should not be null
 *
 * Notes:
 *      CRE if UArray2 is null 
 *      Should not return an int value < 0

 ************************/
int UArray2_width(T uarray2)
{
        assert(uarray2);
        return uarray2->width; 
}

/********** UArray2_height ********
 *
 * Takes in a pointer to a UArray2, and returns an integer equal to the
 * height (number of rows) of the two dimensional array.
 *
 * Parameters:
 *      T uarray2		Pointer to a UArray2 to be measured
 *
 * Return: An integer equivalent to the height of the two dimensional array
 *
 * Expects:
 *      The pointer to the UArray2 must not be null
 *
 * Notes:
 *      CRE if UArray2 is null
 *      Should not return an int value < 0

 ************************/
int UArray2_height(T uarray2)
{
        assert(uarray2);
        return uarray2->height;
}

/********** UArray2_size ********
 *
 * Takes in a pointer to a UArray2, and returns an integer equal to the
 * size of the elements stored in the two dimensional array.
 *
 * Parameters:
 *      T uarray2 		Pointer to a UArray2 to be measured
 *
 * Return: An integer equivalent to the size of elements stored in the
 *         two dimensional array.
 *
 * Expects:
 *      The pointer to the UArray2 should not be null
 *
 * Notes:
 *      CRE if UArray2 is null 
 *      Should not return an int value <= 0  

 ************************/
int UArray2_size(T uarray2)
{
        assert(uarray2);
        return uarray2->size;
}

/********** UArray2_at ********
 *
 * For the two dimensional array specified by the first argument,
 * the function returns a void pointer to the element at the column
 * and row specified by the second and third elements.
 *
 * Parameters:
 *      T uarray2		Pointer to a UArray2 to be measured
 *      int col			Column of the element to be accessed
 *      int row			Row of the element to be accessed
 *
 * Return: A void pointer to the element contained at the
 *         specified column and row
 *
 * Expects:
 *      The pointer to the UArray2 should not be null
 *      Column index provided in the second argument needs to be in range 
 *                              (0 <= col < UArrary2_width(uarray2))
 *      Row index provided in the third argument needs to be in range 
 *                              (0 <= row < UArray2_height(uarray2))
 *
 * Notes:
 *      CRE if UArray2 is null
 *      CRE if col out of bounds (if col < 0, if col > UArrary2_width(uarray2))
 *      CRE if row out of bounds (if row < 0, if row > UArray2_height(uarray2))


 ************************/
void *UArray2_at(T uarray2, int i, int j)
{
        assert(uarray2);
        assert(i >= 0 && i < uarray2->width); 
        assert(j >= 0 && j < uarray2->height);
        return UArray_at(row(uarray2, j), i);
}

/********** UArray2_map_col_major ********
 *
 * Iterates over each element of the UArray2 in column major order, calling
 * the apply function for each element
 *
 * Parameters:
 *      T uarray2			Pointer to a UArray2 to be measured
 *
 *      void apply(int col, int row,            Function to be applied on the
 *      UArray2_T a, void *value, void *cl)	UArray2 once for each element 
 *
 *      void *cl			void pointer to a closure variable
 *
 * Return: None. The function uses the apply function to conduct
 *         certain operations on the 2D array's elements.
 *
 * Expects:
 *      Pointer to the UArray2 should not be null
 *      apply() should not be null and should have the correct parameter types
 *
 * Notes:
 *      CRE if UArray2 or apply() is null
 *      CRE if col/row are not in the correct bounds
 *      void *cl is passed into the apply function, and can be
 *      modified across many apply executions

 ************************/
void UArray2_map_col_major(T uarray2, void apply(int col, int row, UArray2_T a,
void *value, void *cl), void *cl)
{
        assert(uarray2);
        assert(apply);

        for (int col = 0; col < uarray2->width; col++) {
            for (int row = 0; row < uarray2->height; row++) {
                apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
            }
        }
}

/********** UArray2_map_row_major ********
 *
 * Iterates over each element of the UArray2 in row major order, calling
 * the apply function for each element
 *
 * Parameters:
 *      T uarray2			Pointer to a UArray2 to be measured
 *
 *      void apply(int col, int row,            Function to be applied on the
 *      UArray2_T a, void *value, void *cl)	UArray2 once for each element 
 *
 *      void *cl			void pointer to a closure variable
 *
 * Return: None. The function uses the apply function to conduct
 *         certain operations on the 2D array's elements.
 *
 * Expects:
 *      Pointer to the UArray2 should not be null
 *      apply() should not be null and should have the correct parameter types
 *
 * Notes:
 *      CRE if UArray2 or apply() is null
 *      CRE if col/row are not in the correct bounds
 *      void *cl is passed into the apply function, and can be
 *      modified across many apply executions

 ************************/
void UArray2_map_row_major(T uarray2, void apply(int col, int row, UArray2_T a,
void *value, void *cl), void *cl)
{
        assert(uarray2);
        assert(apply);
        
        for (int row = 0; row < uarray2->height; row++) {
            for (int col = 0; col < uarray2->width; col++) {
                apply(col, row, uarray2, UArray2_at(uarray2, col, row), cl);
            }
        }
}

static inline UArray_T row(T a, int j)
{
    UArray_T *prow = UArray_at(a->data, j);
    return *prow;
}

static int is_ok(T a)
{
    return a && UArray_length(a->data) == a->height &&
           UArray_size(a->data) == sizeof(UArray_T) &&
           (a->height == 0 || (UArray_length(row(a, 0)) == a->width &&
                               UArray_size(row(a, 0)) == a->size));
}

#undef T