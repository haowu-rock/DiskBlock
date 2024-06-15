/*
  ==============================================================================

    TransferFunctions.cpp
    Created: 30 Mar 2024 3:00:23pm
    Author:  Hao Wu

  ==============================================================================
*/

#include "TransferFunctions.h"

namespace TransferFunctions {

float tanh(float x) { return (std::exp(2*x) - 1) / (std::exp(2*x) + 1); }

float fuzzexp(float x)
{
    if (x>=0) {
        return -(1 - std::exp(-x)) / (std::exp(1) - 1);
    } else {
        return (1 - std::exp(x)) / (std::exp(1) - 1);;
    }
    return x;
}

float hard(float x) 
{
    if (x > 0.5)
        return 0.5;
    else if (x < -0.5)
        return  -0.5;
    return x;
}

float arraya(float x) { return (1.5*x)*(1-(x*x)/3); }

float cubic(float x) { return x - (x*x*x)/3; }

// y[n] = (2/π) · arctan(α · x[n])
float arctan(float x) { return (0.63661977236) * atan(5*x); }

float soft(float x)
{
    const float th = 1.f/3.f;
    
    if (abs(x) < th) return 2*x;
    if (abs(x) > 2*th) return x>0 ? 1 : -1;
    
    // th <= abs(x) <= 2*th
    if (x > 0) return (3-(2-3*x)*(2-3*x))/3;
    else return -(3-(2-3*abs(x))*(2-3*abs(x)))/3;
}

}
