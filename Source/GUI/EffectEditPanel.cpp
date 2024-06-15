/*
  ==============================================================================

    EffectEditPanel.cpp
    Created: 30 Mar 2024 4:00:49pm
    Author:  Hao Wu

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EffectEditPanel.h"

namespace Gui {


//==============================================================================
EffectEditPanel::EffectEditPanel(RangedAudioParameter &bypassParam, RangedAudioParameter &preGainParam,
                             RangedAudioParameter &postGainParam, RangedAudioParameter &lowcutFreqParam,
                             RangedAudioParameter &highcutFreqParam, RangedAudioParameter &mixParam,
                             std::function<void()>&& deleteAct)
        : bypassAttach(bypassParam, bypassButton),
          preGainAttach(preGainParam, preGainSlider),
          postGainAttach(postGainParam, postGainSlider),
          lowcutFreqAttach(lowcutFreqParam, lowcutFreqSlider),
          highcutFreqAttach(highcutFreqParam, highcutFreqSlider),
          mixAttach(mixParam, mixSlider),
          deleteAction(std::move(deleteAct))
//    : typeSupplier(std::move(typeSupplier))
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    deleteButton.setButtonText("DELETE");
    deleteButton.onClick = [this]() {  deleteAction(); };
    deleteButton.setColour(TextButton::buttonColourId, Colours::red);
    deleteButton.setColour(TextButton::textColourOnId, Colours::white);
    addChildComponent(deleteButton);
    bypassLabel.setText("Bypass:", dontSendNotification);
    bypassLabel.setFont(20);
    bypassLabel.attachToComponent(&bypassButton, true);
    addChildComponent(bypassLabel);
    addChildComponent(bypassButton);
    
    mixLabel.setText("Mix:", dontSendNotification);
    mixLabel.setFont(20);
    mixLabel.attachToComponent(&mixSlider, true);
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addChildComponent(mixLabel);
    addChildComponent(mixSlider);
    
    preGainLabel.setText("Pre Gain:", dontSendNotification);
    preGainLabel.setFont(20);
    preGainLabel.attachToComponent(&preGainSlider, true);
    preGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    preGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addChildComponent(preGainLabel);
    addChildComponent(preGainSlider);
    
    postGainLabel.setFont(20);
    postGainLabel.setText("Post Gain:", dontSendNotification);
    postGainLabel.attachToComponent(&postGainSlider, true);
    postGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    postGainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addChildComponent(postGainLabel);
    addChildComponent(postGainSlider);
    
    lowcutFreqLabel.setText("LowCut Freq:", dontSendNotification);
    lowcutFreqLabel.setFont(20);
    lowcutFreqLabel.attachToComponent(&lowcutFreqSlider, true);
    lowcutFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    lowcutFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addChildComponent(lowcutFreqLabel);
    addChildComponent(lowcutFreqSlider);
    
    highcutFreqLabel.setFont(20);
    highcutFreqLabel.setText("HighCut Freq:", dontSendNotification);
    highcutFreqLabel.attachToComponent(&highcutFreqSlider, true);
    highcutFreqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highcutFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    addChildComponent(highcutFreqLabel);
    addChildComponent(highcutFreqSlider);
    
    // set default
    updateVisibility();
}

EffectEditPanel::~EffectEditPanel()
{
}

void EffectEditPanel::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
}

void EffectEditPanel::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    auto bounds = getLocalBounds().toFloat();
    const float inset = 10;
    
    const float leadingInset = 80 + inset;
//    const float labelW = 40;
//    const float labelH = 20;
    
//    const float bypassH = 30;
//    const float bypassW = 30;
//    bypassLabel.setBounds(inset, inset, labelW, labelH);
//    bypassButton.setBounds(bypassLabel.getBounds().getRight() + inset, inset, bypassW, bypassH);
//    
//    const float sliderW = bounds.getWidth() - 2*inset;
//    const float sliderH = 30;
//    preGainLabel.setBounds(inset, bypassButton.getBounds().getBottom() + inset, labelW, labelH);
//    preGainSlider.setBounds(preGainLabel.getBounds().getRight() + inset, preGainLabel.getBounds().getY(), sliderW, sliderH);
//    postGainLabel.setBounds(inset, preGainSlider.getBounds().getBottom() + inset, labelW, labelH);
//    postGainSlider.setBounds(postGainLabel.getBounds().getRight() + inset, postGainLabel.getBounds().getY(), sliderW, sliderH);
    
    const float bypassH = 30;
    const float bypassW = 30;
    bypassButton.setBounds(leadingInset, inset, bypassW, bypassH);
    
    const float deleteW = 100;
    const float deleteH = 30;
    deleteButton.setBounds(bounds.getWidth() - inset - deleteW, inset, deleteW, deleteH);
    
    const float sliderW = bounds.getWidth() - inset - leadingInset;
    const float sliderH = 30;
    mixSlider.setBounds(leadingInset, bypassButton.getBounds().getBottom() + inset, sliderW, sliderH);
    preGainSlider.setBounds(leadingInset, mixSlider.getBounds().getBottom() + inset, sliderW, sliderH);
    postGainSlider.setBounds(leadingInset, preGainSlider.getBounds().getBottom() + inset, sliderW, sliderH);
    lowcutFreqSlider.setBounds(leadingInset, postGainSlider.getBounds().getBottom() + inset, sliderW, sliderH);
    highcutFreqSlider.setBounds(leadingInset, lowcutFreqSlider.getBounds().getBottom() + inset, sliderW, sliderH);
    
    deleteButton.setBounds(inset, bounds.getHeight() - inset - 30, bounds.getWidth() - 2*inset, 30);
}

void EffectEditPanel::setType(SlotType t)
{
//    if (type == t) {
//        return;
//    }
    type = t;
    
    updateVisibility();
}

void EffectEditPanel::updateVisibility()
{
    if (type == IDLE)
    {
        deleteButton.setVisible(false);
        bypassLabel.setVisible(false);
        bypassButton.setVisible(false);
        preGainLabel.setVisible(false);
        preGainSlider.setVisible(false);
        postGainLabel.setVisible(false);
        postGainSlider.setVisible(false);
        mixLabel.setVisible(false);
        mixSlider.setVisible(false);
        lowcutFreqLabel.setVisible(false);
        lowcutFreqSlider.setVisible(false);
        highcutFreqLabel.setVisible(false);
        highcutFreqSlider.setVisible(false);
    }
    else if (type == DIST)
    {
        deleteButton.setVisible(true);
        bypassLabel.setVisible(true);
        bypassButton.setVisible(true);
        preGainLabel.setVisible(true);
        preGainSlider.setVisible(true);
        postGainLabel.setVisible(true);
        postGainSlider.setVisible(true);
        mixLabel.setVisible(true);
        mixSlider.setVisible(true);
        lowcutFreqLabel.setVisible(true);
        lowcutFreqSlider.setVisible(true);
        highcutFreqLabel.setVisible(true);
        highcutFreqSlider.setVisible(true);
    }
    else if (type == IR)
    {
        deleteButton.setVisible(true);
        bypassLabel.setVisible(true);
        bypassButton.setVisible(true);
        preGainLabel.setVisible(false);
        preGainSlider.setVisible(false);
        postGainLabel.setVisible(false);
        postGainSlider.setVisible(false);
        mixLabel.setVisible(true);
        mixSlider.setVisible(true);
        lowcutFreqLabel.setVisible(false);
        lowcutFreqSlider.setVisible(false);
        highcutFreqLabel.setVisible(false);
        highcutFreqSlider.setVisible(false);
    }
}

} //namespace
