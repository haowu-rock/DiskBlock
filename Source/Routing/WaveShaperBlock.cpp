/*
  ==============================================================================

    WaveShaperBlock.cpp
    Created: 30 Mar 2024 1:16:01pm
    Author:  Hao Wu

  ==============================================================================
*/

#include "WaveShaperBlock.h"
#include <iostream>

namespace Routing
{

WaveShaperBlock::WaveShaperBlock(float (*transferFunction)(float), std::string name)
            : Block(DIST, name, 1.0), useStaturation(true)
{
//    std::cout << "Constructor called for object at: " << this << std::endl;

    auto& waveshaper = processorChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = transferFunction;
    auto& preGain = processorChain.template get<preGainIndex>();
    preGain.setGainDecibels (0.f);
    auto& postGain = processorChain.template get<postGainIndex>();
    postGain.setGainDecibels (0.f);
}

WaveShaperBlock::~WaveShaperBlock()
{
    std::cout << "WaveShaperBlock Destructor called for object at: " << this << std::endl;
}


void WaveShaperBlock::prepare(double sampleRate, int samplesPerBlock)
{
    fs = sampleRate;
    
    auto& lowCut = processorChain.template get<lowCutIndex>();
    lowCut.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, lowcutFreq);
    lowCut.reset();
    auto& highCut = processorChain.template get<highCutIndex>();
    highCut.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, highcutFreq);
    highCut.reset();
//    auto& bandPass = processorChain.template get<bandPass>();                      // [3]
//    bandPass.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(<#double sampleRate#>, <#float frequency#>)(sampleRate, 1000.0f);  // [4]
    
    
    processorChain.prepare({sampleRate, static_cast<uint32>(samplesPerBlock), 1});
    blockSize = samplesPerBlock;
    wetSamples = (float*)malloc(sizeof(float) * blockSize);
}

void WaveShaperBlock::releaseResources()
{
    if (wetSamples) 
    {
        free(wetSamples);
        wetSamples = 0;
    }
}

void WaveShaperBlock::process(float *sampels, int numSamples)
{
    if (bypass)
        return;
    
    // copy samples for processing
    memcpy(wetSamples, sampels, sizeof(float) * blockSize);

    juce::dsp::AudioBlock<float> audioBlock(&wetSamples, 1, numSamples);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
//    processorChain.process(context);
    
    auto& preGain = processorChain.template get<preGainIndex>();
    preGain.process(context);
    memcpy(sampels, wetSamples, sizeof(float) * numSamples);
    
    auto& lowCut = processorChain.template get<lowCutIndex>();
    lowCut.process(context);
    auto& highCut = processorChain.template get<highCutIndex>();
    highCut.process(context);
    
    auto& waveshaper = processorChain.template get<waveshaperIndex>();
    waveshaper.process(context);
    for (int i = 0; i < blockSize; ++i) {
        wetSamples[i] = mix * wetSamples[i] + (1-mix)*sampels[i];
    }
//    waveshaper.functionToUse = transferFunction;
    
//    preGain.setGainDecibels (0.f);
    auto& postGain = processorChain.template get<postGainIndex>();
    postGain.process(context);
    memcpy(sampels, wetSamples, sizeof(float) * blockSize);
    
    for (int i = 0; i < blockSize; ++i)
    {
        if (sampels[i] > 1)
            sampels[i] = 1.f;
        
    }
//    if (!bypass) {
//        for (int i = 0; i < blockSize; ++i) {
//            sampels[i] = mix * wetSamples[i] + (1-mix)*sampels[i];
//        }
//    }
    
//    printf("<< %f\n", sampels[1]);
}

float WaveShaperBlock::getPreGainDB()
{
    return processorChain.template get<preGainIndex>().getGainDecibels();
}
float WaveShaperBlock::getPostGainDB()
{
    return processorChain.template get<postGainIndex>().getGainDecibels();
}
void WaveShaperBlock::setPreGainDB(float db)
{
    processorChain.template get<preGainIndex>().setGainDecibels(db);
}
void WaveShaperBlock::setPostGainDB(float db)
{
    processorChain.template get<postGainIndex>().setGainDecibels(db);
}

void WaveShaperBlock::setLowcutFreq(float f)
{
    lowcutFreq = f;
    processorChain.template get<lowCutIndex>().coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(fs, f);
}
void WaveShaperBlock::setHighcutFreq(float f)
{
    highcutFreq = f;
    processorChain.template get<highCutIndex>().coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(fs, f);
}

} // namespace
