/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./GUI/VerticalGridMeter.h"
#include "./GUI/EffectEditPanel.h"
#include "./GUI/FlowChainView.h"
#include "./GUI/GuiCommon.h"
#include "./GUI/NewBlockParamView.h"
//==============================================================================
/**
*/
class DistBlockAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    DistBlockAudioProcessorEditor (DistBlockAudioProcessor&);
    ~DistBlockAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

//    void setSelectionChangedAction(std::function<void(int)>&& action);
//    int getSelectedBlockIndex() { return selectedIndex; }
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    DistBlockAudioProcessor& audioProcessor;
    
    ViewModel::FlowChainViewModel chainViewModel;
    
    Gui::VerticalGridMeter inputMeter;
    Gui::VerticalGridMeter outputMeter;
    
    Label inGainLabel;
    Slider inGainSlider;
    Label outGainLabel;
    Slider outGainSlider;
    juce::SliderParameterAttachment inGainAttach;
    juce::SliderParameterAttachment outGainAttach;
    
    Gui::EffectEditPanel editPanel;
    Gui::FlowChainView chainView;
    
    Gui::NewBlockParamView newBlockParamView;
//    int currentBlock/*Index { 0 };*/
    
//    std::function<void(int)> selectionChangedAction;
    int selectedIndex = -1;
    
    void clickedAtBlock(int index);
    void updateSelectedIndex(int index);
    void findAndNewIndex();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistBlockAudioProcessorEditor)
};
