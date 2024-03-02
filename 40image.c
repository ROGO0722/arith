#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "a2methods.h"
#include "pnm.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "mem.h"

typedef struct converted_values
{
        float value1, value2, value3; 
} *converted_values;

typedef struct Pnm_float {
        int width, height, denominator;
        UArray2_T array;          /* 2D array with
                                            * element type 'struct Pnm_rgb' 
                                            */
} *Pnm_float;

typedef struct floats_for_32bit {
        float avg_Pb, avg_Pr, a, b, c, d;
} *floats_for_32bit;

static void (*compress_or_decompress)(FILE *input) = compress40;
void compress40(FILE *input);
void decompress40(FILE *input);
Pnm_ppm trim_image(FILE *input, A2Methods_T methods);
Pnm_ppm trim_helper(Pnm_ppm image, A2Methods_T methods);
Pnm_float RGB_to_RGBfloat(Pnm_ppm image, A2Methods_T methodss);
Pnm_ppm RGBfloat_to_RGB(Pnm_float data, A2Methods_T methods);
Pnm_float Rgbfloat_to_YPbPr(Pnm_float data, A2Methods_T methods);
Pnm_float YPbPr_to_Rgbfloat(Pnm_float data, A2Methods_T methods);
Pnm_float YPbPr_to_floats_for_32bit(Pnm_float data, A2Methods_T methods);

int main(int argc, char *argv[])
{

        int i;
    
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }
        // FILE *fp = fopen(argv[i], "r");
        // Pnm_ppm image = trim_image(fp);
        // assert(image != NULL);
        // fclose(fp);
        return EXIT_SUCCESS; 
}

void compress40(FILE *input) {
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);
        Pnm_ppm image = trim_image(input, methods);
        assert(image != NULL);
        //compression step 1
        Pnm_float data = RGB_to_RGBfloat(image, methods);
        //compression step 2
        data = Rgbfloat_to_YPbPr(data, methods);
        //compression step 3
        data = YPbPr_to_floats_for_32bit(data, methods);
        //decompression step 3
        // data = YPbPr_to_Rgbfloat(data, methods); 
        //decompression step 4
        // Pnm_ppm decompressed_image = RGBfloat_to_RGB(data, methods); 
        // Pnm_ppmwrite(stdout, decompressed_image);

        Pnm_ppmfree(&image);
        // Pnm_ppmfree(&decompressed_image);
        UArray2_free(&(data->array));
        free(data);
        // free(data);
        // FREE(&data);
}

void decompress40(FILE *input) {
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);
        Pnm_ppm image = trim_image(input, methods);
        assert(image != NULL);
        Pnm_ppmwrite(stdout, image);
        // (void) input;
        // return;
}

Pnm_ppm trim_image(FILE *input, A2Methods_T methods) {
        Pnm_ppm image = Pnm_ppmread(input, methods);
        assert(image != NULL);
        // printf("The image's width is %d, and height is %d\n", image->width, image->height);
        int modify = 0;
        if(image->width % 2 == 1) {
                image->width -= 1;
                modify = 1;
        } 
        if(image->height %2 == 1) {
                image->height -= 1;
                modify =1;
        }
        if(modify) {
                return trim_helper(image, methods);
        } else {
                return image;
        }
}


Pnm_ppm trim_helper(Pnm_ppm image, A2Methods_T methods) {
        Pnm_ppm trimmed_image;
        NEW(trimmed_image);
        assert(trimmed_image != NULL);
        trimmed_image->width = image->width;
        trimmed_image->height = image->height; 
        trimmed_image->pixels = methods->new(image->width, image->height, 
                                             sizeof(struct Pnm_rgb));
        trimmed_image->denominator = image->denominator; 
        trimmed_image->methods = methods; 
        // printf("The trimmed image's width is %d, and height is %d\n", 
        // trimmed_image->width, trimmed_image->height); 
        for(int row = 0; row < (int) trimmed_image->height; row++) {
                for(int col = 0; col < (int) trimmed_image->width; col++) {
                        Pnm_rgb orig = methods->at(image->pixels, col ,row);  
                        // printf("Original values are %u, %u, %u\n", orig->red, orig->green, orig->blue);
                        Pnm_rgb trimmed = methods->at(trimmed_image->pixels, 
                                                      col, row); 
                        trimmed->red = orig->red;
                        trimmed->green = orig->green;
                        trimmed->blue = orig->blue;
                        // printf("Values are %u, %u, %u\n", trimmed->red, 
                        //        trimmed->green, trimmed->blue);
                                
                }
        }  
        Pnm_ppmfree(&image);
        return trimmed_image;
}
//compression step 1
Pnm_float RGB_to_RGBfloat(Pnm_ppm image, A2Methods_T methods) {
        Pnm_float data = NEW(data);
        assert(data != NULL);
        data->width = (int) image->width;
        data->height = (int) image->height;
        data->denominator = (int) image->denominator;
        int sizeStruct = sizeof(struct converted_values);
        UArray2_T arr = UArray2_new(data->width, data->height, sizeStruct);
        for(int row = 0; row < data->height; row++) {
                for(int col = 0; col < data->width; col++) {
                        Pnm_rgb orig = methods->at(image->pixels, col ,row);
                        // converted_values values = malloc(sizeStruct); 
                        // assert(values != NULL);
                        /* Stores the converted red value in value1 */
                        converted_values values =  (converted_values)        
                                                   UArray2_at(arr, col, row);
                        assert(orig != NULL);
                        values->value1 = (float) (orig->red) / 
                                         data->denominator;
                        /* Stores the converted green value in value2 */
                        values->value2 = (float) (orig->green) / 
                                         data->denominator;
                        /* Stores the converted blue value in value3 */
                        values->value3 = (float) (orig->blue) / 
                                         data->denominator; 
                        // * ((converted_values *) UArray2_at(arr, col, row)) = 
                        // values;
                        // printf("NewValues are red flaot: %f, green float: %f, blue float: %f\n", values->value1, values->value2, values->value3);
                }
        }  
        data->array = arr;
        assert(data != NULL);
        return data;
}

//compression step 2
Pnm_float Rgbfloat_to_YPbPr(Pnm_float data, A2Methods_T methods) {
        Pnm_float newData =  NEW(newData);
        assert(newData != NULL);
        newData->width = data->width;
        newData->height = data->height;
        newData->denominator = data->denominator;
        int sizeStruct = sizeof(struct converted_values);
        UArray2_T arr = UArray2_new(newData->width, newData->height, 
                                    sizeStruct);
        for(int row = 0; row < newData->height; row++) {
                for(int col = 0; col < newData->width; col++) {
                        converted_values rgb_float = methods->at(data->array, 
                                                                 col ,row);
                        
                        converted_values values =  (converted_values)        
                                                   UArray2_at(arr, col, row);
                        assert(rgb_float != NULL);
                        /* Stores the calculated Y value in value1 */
                        values->value1 = (0.299 * rgb_float->value1) + 
                                                (0.587 * rgb_float->value2) + 
                                                (0.114 * rgb_float->value3);
                        
                        /* Stores the calculated pb value in value2 */
                        values->value2 =  -0.168736 * rgb_float->value1 - 
                                          0.331264 * rgb_float->value2 + 0.5 * 
                                          rgb_float->value3;
                        /* Stores the calculated pr value in value3 */
                        values->value3 =  0.5 * rgb_float->value1 - 
                                          0.418688 * rgb_float->value2 - 
                                          0.081312 * rgb_float->value3;
        
                        // printf("NewValues are Y: %f, Pb: %f, Pr: %f\n", values->value1, values->value2, values->value3);                   
                }
        }  
        int width = UArray2_width(arr)/2;
        int height = UArray2_height(arr)/2;
        for (int i = 0; i < width * height; i++) {
                /* finding index to access each block in UArray2b_T */
                int boxRow = (i / width) * 2;
                int boxCol = (i % width) * 2;  

                 /* mapping through the box using row major */              
                for (int j = boxRow; j < boxRow + 2; j++) {
                        for (int k = boxCol; k < boxCol + 2; k++) {
                                 converted_values values =  (converted_values)  UArray2_at(arr, k, j);
                                printf("NewValues are Y: %f, Pb: %f, Pr: %f\n", values->value1, values->value2, values->value3);               
                        }
                }
        }      
        newData->array = arr; 
        assert(newData != NULL);
        UArray2_free(&(data->array));
        free(data);
        return newData; 
}

//compression step3
Pnm_float YPbPr_to_floats_for_32bit(Pnm_float data, A2Methods_T methods) {
        (void)methods;
        Pnm_float newData =  NEW(newData);
        assert(newData != NULL);
        newData->width = data->width;
        newData->height = data->height;
        newData->denominator = data->denominator;
        UArray2_T arr = UArray2_new(newData->width / 2, newData->height / 2, 
                                    sizeof(struct floats_for_32bit));

        for (int i = 0; i < UArray2_height(arr) * UArray2_width(arr); i++) {
                 /* finding index to access each block in UArray2b_T */
                int boxRow = (i / UArray2_width(arr));
                int boxCol = (i % UArray2_width(arr));
              
                /* mapping through the box using row major */ 
                floats_for_32bit values = (floats_for_32bit) 
                                          UArray2_at(arr, boxCol, boxRow);  

                int counter = 0;
                float y1, y2, y3, y4; 
                for (int j = boxRow * 2; j < boxRow * 2 + 2; j++) {
                        for (int k = boxCol * 2; k < boxCol * 2 + 2; k++) {
                                converted_values temp = (converted_values) 
                                                        UArray2_at
                                                        (data->array, 
                                                        k, j);
                                values->avg_Pb += temp->value2; 
                                values->avg_Pr += temp->value3; 
                                if(counter == 0) {
                                        y1 = temp->value1;
                                } else if(counter == 1) {
                                        y2 = temp->value1;
                                } else if(counter == 2) {
                                        y3 = temp->value1;
                                } else {
                                        y4 = temp->value1;
                                }
                                counter++;
                        }
                }
                values->avg_Pb /= 4;
                values->avg_Pr /= 4; 
                values->a = (y4 + y3 + y2 + y1) / 4;
                values->b = (y4 + y3 - y2 - y1) / 4;
                values->c = (y4 - y3 + y2 - y1) / 4;
                values->d = (y4 - y3 - y2 - y1) / 4; 
                // printf("Values are avg_Pb: %f, avg_Pr: %f, a: %f, b: %f\n , c: %f, d: %f\n", values->avg_Pb, values->avg_Pr, values->a, values->b, values->c, values->d);
                                                                
        }  
        newData->array = arr; 
        assert(newData != NULL);
        UArray2_free(&(data->array));
        free(data);
        return newData; 
}

//decompression 4
Pnm_ppm RGBfloat_to_RGB(Pnm_float data, A2Methods_T methods) {
        Pnm_ppm image = NEW(image); 
        assert(image);
        image->width = data->width;
        image->height = data->height;
        image->denominator = data->denominator;
        image->methods = methods;
        image->pixels = methods->new(image->width, image->height, 
                                             sizeof(struct Pnm_rgb));

        for(int row = 0; row < data->height; row++) {
                for(int col = 0; col < data->width; col++) {
                        converted_values value = (converted_values)methods->at(data->array, col ,row);
                        // printf("DENOMINATOR IS %d", denominator);
                        // Pnm_rgb orig = malloc(sizeof(struct Pnm_rgb));
                        Pnm_rgb orig = methods->at(image->pixels, col, row);
                        assert(orig != NULL);
                        // printf("values are currently %f, %f, %f\n", value->value1, value->value2, value->value3);
                        orig->red = (int) ((value->value1) * data->denominator);
                        orig->green = (int) ((value->value2) * 
                                             data->denominator); 
                        orig->blue = (int) ((value->value3) * 
                                            data->denominator); 
                        // printf("Values are %d, %d, %d\n", orig->red, orig->green, orig->blue);
                //         * ((Pnm_rgb *) methods->at(image->pixels, col, row)) = orig;
                }
        } 
        return image; 
}
//decompression step 3
Pnm_float YPbPr_to_Rgbfloat(Pnm_float data, A2Methods_T methods) {
        Pnm_float newData =  NEW(newData);
        assert(newData != NULL);
        newData->width = data->width;
        newData->height = data->height;
        newData->denominator = data->denominator;
        int sizeStruct = sizeof(struct converted_values);
        UArray2_T arr = UArray2_new(newData->width, newData->height, 
                                    sizeStruct);
        for(int row = 0; row < newData->height; row++) {
                for(int col = 0; col < newData->width; col++) {
                        converted_values yPbPr_float = methods->at(data->array, 
                                                                 col ,row);
                        
                        converted_values values =  (converted_values)        
                                                   UArray2_at(arr, col, row);
                        assert(yPbPr_float != NULL);
                        /* Stores the calculated red float value in value1 */
                        values->value1 = yPbPr_float->value1 + 
                                                1.402 * yPbPr_float->value3;
                        
                        /* Stores the calculated green float value in value2 */
                        values->value2 =  yPbPr_float->value1 - 
                                          0.344136 * yPbPr_float->value2 - 0.714136 * yPbPr_float->value3;
                        /* Stores the calculated blue float value in value3 */
                        values->value3 =  yPbPr_float->value1 + 
                                          1.772 * yPbPr_float->value2; 
                        // printf("NewValues after decompression are Red: %f, Green: %f, Blue: %f\n", values->value1, values->value2, values->value3);
                }
        }  
        newData->array = arr; 
        assert(newData != NULL);
        UArray2_free(&(data->array));
        free(data);
        return newData; 
}

//decompression step 2
Pnm_float floats_for_32bit_to_YPbPr(Pnm_float data, A2Methods_T methods) {
        Pnm_float newData =  NEW(newData);
        assert(newData != NULL);
        newData->width = data->width;
        newData->height = data->height;
        newData->denominator = data->denominator;
        int sizeStruct = sizeof(struct converted_values);
        UArray2_T arr = UArray2_new(newData->width, newData->height, 
                                    sizeStruct);
        for(int row = 0; row < newData->height; row++) {
                for(int col = 0; col < newData->width; col++) {
                        converted_values yPbPr_float = methods->at(data->array, 
                                                                 col ,row);
                        
                        converted_values values =  (converted_values)        
                                                   UArray2_at(arr, col, row);
                        assert(yPbPr_float != NULL);
                        /* Stores the calculated red float value in value1 */
                        values->value1 = yPbPr_float->value1 + 
                                                1.402 * yPbPr_float->value3;
                        
                        /* Stores the calculated green float value in value2 */
                        values->value2 =  yPbPr_float->value1 - 
                                          0.344136 * yPbPr_float->value2 - 0.714136 * yPbPr_float->value3;
                        /* Stores the calculated blue float value in value3 */
                        values->value3 =  yPbPr_float->value1 + 
                                          1.772 * yPbPr_float->value2; 
                        // printf("NewValues after decompression are Red: %f, Green: %f, Blue: %f\n", values->value1, values->value2, values->value3);
                }
        }  
        newData->array = arr; 
        assert(newData != NULL);
        UArray2_free(&(data->array));
        free(data);
        return newData; 
}











// void YPbPr_to_floats_for_32bit(Pnm_float data, A2Methods_T methods) {
//         Pnm_float newData =  NEW(newData);
//         assert(newData != NULL);
//         UArray2b_T packedArray = UArray2b_new(data->width, data->height, sizeof
//                                               (struct converted_values), 2);
//         assert(packedArray);
//         newData->width = data->width;
//         newData->height = data->height;
//         newData->denominator = data->denominator;
//         // int height = packedArray->height;
//         // int sizeStruct = sizeof(struct converted_values);
//         UArray2_T arr = UArray2_new(newData->width / 2, newData->height / 2, 
//                                     sizeof(struct floats_for_32bit));
//         (void) arr;
//         for(int row = 0; row < newData->height; row++) {
//                 for(int col = 0; col < newData->width; col++) {
//                         converted_values YPbPr = UArray2b_at(packedArray, 
//                                                              col ,row);
//                         converted_values fromData = methods->at(data->array, 
//                                                                 col, row);
//                         YPbPr->value1 = fromData->value1;
//                         YPbPr->value2 = fromData->value2;
//                         YPbPr->value3 = fromData->value3;
//                 }
//         } 
//         for(int row = 0; row < newData->height; row++) {
//                 for(int col = 0; col < newData->width; col++) {
//                         converted_values values =  UArray2b_at(packedArray, 
//                                                         col ,row);
//                         printf("NewValues are Y: %f, Pb: %f, Pr: %f\n", values->value1, values->value2, values->value3);
//                 }
//         }  
//         // newData->array = arr; 
//         // assert(newData != NULL);
//         // UArray2_free(&(data->array));
//         // free(data);
//         // return newData; 
// }

