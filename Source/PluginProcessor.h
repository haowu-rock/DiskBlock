/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "./Routing/Manager.h"
#include <memory>
//==============================================================================
/**
*/
const std::string kDIST_GLOBAL_IN_GAIN_ID = "GLOBAL_IN_GAIN";
const std::string kDIST_GLOBAL_OUT_GAIN_ID = "GLOBAL_OUT_GAIN";
const std::string kDIST_PRE_GAIN_ID = "DIST_PRE_GAIN";
const std::string kDIST_POST_GAIN_ID = "DIST_POST_GAIN";
const std::string kDIST_LOWCUT_FREQ_ID = "DIST_LOWCUT_FREQ";
const std::string kDIST_HIGHCUT_FREQ_ID = "DIST_HIGHCUT_FREQ";
const std::string kEFFECT_BYPASS_ID = "EFFECT_BYPASS";
const std::string kEFFECT_MIX_ID = "EFFECT_MIX";

class DistBlockAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DistBlockAudioProcessor();
    ~DistBlockAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float getInputRmsDB() { return inputLevelDB.getCurrentValue(); }
    float getOutputRmsDB() { return outputLevelDB.getCurrentValue(); }
    
    Routing::Manager& getFlowManager() { return flowManager; }
    
    AudioProcessorValueTreeState& getParamState() { return vts; }
    
    void setSelectedBlockIndex(int index);
    
private:
    Routing::Manager flowManager;
    LinearSmoothedValue<float> inputLevelDB, outputLevelDB;
    
    AudioProcessorValueTreeState vts;
    
    std::atomic<float>* globalInGain = nullptr;
    std::atomic<float>* globalOutGain = nullptr;
    
    std::atomic<float>* preGain = nullptr;
    std::atomic<float>* postGain = nullptr;
    std::atomic<float>* mixing = nullptr;
    std::atomic<float>* bypass = nullptr;
    
    std::atomic<float>* distLowCutFreq = nullptr;
    std::atomic<float>* distHighCutFreq = nullptr;
    
    std::shared_ptr<Routing::WaveShaperBlock> curWaveShaperBlock;
    std::shared_ptr<Routing::IRBlock> curIRBlock;
    
    juce::dsp::Gain<float> inGainProcesser;
    juce::dsp::Gain<float> outGainProcesser;
    
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistBlockAudioProcessor)
};
