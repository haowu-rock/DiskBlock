//
//  convolution.c
//  IRLoader - VST3
//
//  Created by Hao Wu on 2022/7/29.
//

#include "convolution.h"
#include <stdlib.h>
#include <math.h>
struct convolution
{
    int length;
    
    float *input;
    int p_w;
};


convolution_ptr convolution_alloc(int length)
{
    convolution_ptr conv = (convolution_ptr)malloc(sizeof(struct convolution));
    conv->p_w = 0;
    conv->length = length;
    conv->input = (float *)calloc(length, sizeof(float));
    
    return conv;
}

void convolution_dealloc(convolution_ptr conv)
{
    free(conv->input);
    free(conv);
}

void convolution_process(convolution_ptr conv, float* sample, int size, float *ir)
{
//    printf("size:%d\n", size);
    for (int i = 0; i < size; i++)
    {
        conv->input[conv->p_w] = sample[i];
        conv->p_w += 1;
        if (conv->p_w >= conv->length)
        {
            conv->p_w = 0;
        }
        
        float sum = 0;
        int p_r = conv->p_w-1;
        for (int t = 0; t < conv->length; t++)
        {
            // not check bounds
            if (p_r < 0)
            {
                p_r = conv->length - 1;
            }
            
            sum += conv->input[p_r] * ir[t];
            
            p_r -= 1;
        }
        
        sample[i] = sum;
        
        if (fabsf(sample[i]) > 1)
        {
//            printf("%d - %f\n", i, sample[i]);
            sample[i] = 1;
        }
        
        
    }
}
