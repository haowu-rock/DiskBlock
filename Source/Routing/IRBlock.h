/*
  ==============================================================================

    IRBlock.h
    Created: 3 Apr 2024 7:13:36pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Block.h"
#include <memory>


namespace Routing
{

class IRBlock: public Block
{
public:
//    IRBlock(std::string irPath, std::string name);
    IRBlock(File irFile, std::string name, int irSize, float defaultMix);
    ~IRBlock() override;
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void process(float *sampels, int numSamples) override;
    
private:
//    static constexpr auto fftOrder = 2;
//    static constexpr int fftSize = 1 << fftOrder; // 2048
    int fftSize { 0 };
    
    float *ir { 0 };
    const int irSize; // { 8192 };
    
    int blockSize { 0 };
    float *wetSamples { 0 };
    
    float *linearOutput { 0 };
    int linearOutSize { 0 };

    int numSubIR { 0 };
    
    std::unique_ptr<juce::dsp::FFT> FFT;
    
    // arrary of sub H(e^jw)
    std::vector<std::vector<std::complex<float>>> subIrFFTData;
    
    // For compute and store sub X(e^jw)
    std::vector<std::complex<float>> XFFTData;
    
    // For compute and store sub Y(e^jw)
    std::vector<std::complex<float>> subYFFTData;
    
    AudioFormatManager formatManager;
    
    void normalizeIR(float *ir, int size);
};

} //namespace
