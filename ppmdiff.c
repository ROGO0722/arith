#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include "assert.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

void find_E(int col, int row, Pnm_ppm image1, Pnm_ppm image2, float* E, 
            A2Methods_T methods);

int main(int argc, char *argv[])
{
        if(argc > 3) {
                return EXIT_FAILURE;
        }

        FILE *fp = fopen(argv[1], "r");
        FILE *fp1 = fopen(argv[2], "r");
        float E = 0;
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL); 
        // A2Methods_mapfun *map = methods->map_default; 
        // assert(map != NULL);

        Pnm_ppm image1 = Pnm_ppmread(fp, methods); 
        Pnm_ppm image2 = Pnm_ppmread(fp1, methods);
        assert(image1 != NULL && image2 != NULL);
        unsigned width1 = image1->width;
        unsigned height1 = image1->height;
        unsigned width2 = image2->width;
        unsigned height2 = image2->height;
        printf("Image 1 Dimensions: %d x %d\n", 
                image1->width, image1->height);
        printf("Image 2 Dimensions: %d x %d\n", 
                image2->width, image2->height);
            
        if(abs((int) width1 - (int) width2 > 1) || (abs((int) height1 - (int) height2 > 1))) {
                fprintf(stderr, "Image dimensions differ greater than 1!\n");
                return EXIT_FAILURE;
        }
        for(int row = 0; row < (int) fmin(height1, height2); row++) {
                for(int col = 0; col < (int) fmin(width1, width2); col++) {
                        find_E(col, row, image1, image2, &E, methods);
                }
        }
        printf("Calculated E is %.4f\n", sqrt(E));
        return EXIT_SUCCESS; 
}

/* diffR = difference of red, diffG = difference of green, diffB = difference 
of blue */
void find_E(int col, int row, Pnm_ppm image1, Pnm_ppm image2, float* E, 
A2Methods_T methods) {
        Pnm_rgb Image1 = methods->at(image1->pixels, col, row);
        Pnm_rgb Image2 = methods->at(image2->pixels, col, row);
        float diffR = ((float) (Image1->red) / (float) (image1->denominator)) - 
                      ((float) (Image2->red) / (float) (image2->denominator));
        float diffG = ((float) (Image1->green) / (float) (image1->denominator)) 
                      - ((float) (Image2->green) / 
                      (float)(image2->denominator));
        float diffB =  ((float)(Image1->blue) / (float)(image1->denominator)) - 
                      ((float)(Image2->blue) / (float)(image2->denominator));
        int height = (int) fmin(image1->height, image2->height);
        int width = (int) fmin(image1->width, image2->width);
        float value = (pow(diffR, 2) + pow(diffG, 2) + pow(diffB, 2));
        float denominator = (float) (3 * height * width);
        *E += (value / denominator);
}