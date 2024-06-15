/*
  ==============================================================================

    EffectEditPanel.h
    Created: 30 Mar 2024 4:00:49pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiCommon.h"

//==============================================================================
/*
*/
namespace Gui {

class EffectEditPanel  : public juce::Component
{
public:
    EffectEditPanel(RangedAudioParameter &bypassParam,
                    RangedAudioParameter &preGainParam,
                    RangedAudioParameter &postGainParam,
                    RangedAudioParameter &lowcutFreqParam,
                    RangedAudioParameter &highcutFreqParam,
                    RangedAudioParameter &mixParam,
                    std::function<void()>&& deleteAct);
    ~EffectEditPanel() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    
    void setType(SlotType t);
    
private:
//    std::function<SlotType(int)> typeSupplier;
    
    SlotType type { IDLE };
    
    TextButton deleteButton;
    
    Label bypassLabel;
    ToggleButton bypassButton;
    
    Label preGainLabel;
    Slider preGainSlider;
    
    Label postGainLabel;
    Slider postGainSlider;
    
    Label lowcutFreqLabel;
    Slider lowcutFreqSlider;
    
    Label highcutFreqLabel;
    Slider highcutFreqSlider;
    
    Label mixLabel;
    Slider mixSlider;

    juce::ButtonParameterAttachment bypassAttach;
    juce::SliderParameterAttachment preGainAttach;
    juce::SliderParameterAttachment postGainAttach;
    juce::SliderParameterAttachment lowcutFreqAttach;
    juce::SliderParameterAttachment highcutFreqAttach;
    juce::SliderParameterAttachment mixAttach;
    
    std::function<void()> deleteAction;
    void updateVisibility();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectEditPanel)
};

} //namespace
