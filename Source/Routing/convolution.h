//
//  convolution.h
//  IRLoader - VST3
//
//  Created by Hao Wu on 2022/7/29.
//

#ifndef convolution_h
#define convolution_h

#include <stdio.h>

typedef struct convolution* convolution_ptr;

convolution_ptr convolution_alloc(int length);
void convolution_dealloc(convolution_ptr conv);
void convolution_process(convolution_ptr conv, float* sample, int size, float *ir);


#endif /* convolution_h */
