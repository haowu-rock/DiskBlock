/*
  ==============================================================================

    WaveShaperBlock.h
    Created: 30 Mar 2024 1:16:01pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Block.h"

namespace Routing
{

class WaveShaperBlock: public Block
{
public:
    WaveShaperBlock(float (*transferFunction)(float), std::string name);
    ~WaveShaperBlock() override;
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void process(float *sampels, int numSamples) override;
    
    float getPreGainDB();
    float getPostGainDB();
    
    void setPreGainDB(float db);
    void setPostGainDB(float db);
    
    void setLowcutFreq(float f);
    void setHighcutFreq(float f);
    float getLowcutFreq() { return lowcutFreq; }
    float getHighcutFreq() { return highcutFreq; }
private:
    enum
    {
        preGainIndex,
        lowCutIndex,
        highCutIndex,
        waveshaperIndex,
        postGainIndex
    };
    
    juce::dsp::ProcessorChain<juce::dsp::Gain<float>,
                              juce::dsp::IIR::Filter<float>,
                              juce::dsp::IIR::Filter<float>,
                              juce::dsp::WaveShaper<float>,
                              juce::dsp::Gain<float>> processorChain;
                              
    
    const bool useStaturation;
    SmoothedValue<float> staturation;
    
    float lowcutFreq { 20.f };
    float highcutFreq { 20000.f };
    
    double fs { 0 };
    int blockSize { 0 };
    float *wetSamples { 0 };
};

} // namespace
