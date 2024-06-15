/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistBlockAudioProcessorEditor::DistBlockAudioProcessorEditor (DistBlockAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        chainViewModel(ViewModel::FlowChainViewModel(audioProcessor.getFlowManager())),
        inputMeter([&](){ return audioProcessor.getInputRmsDB(); }),
        outputMeter([&](){ return audioProcessor.getOutputRmsDB(); }),
        inGainAttach(*p.getParamState().getParameter(kDIST_GLOBAL_IN_GAIN_ID), inGainSlider),
        outGainAttach(*p.getParamState().getParameter(kDIST_GLOBAL_OUT_GAIN_ID), outGainSlider),
        editPanel(*p.getParamState().getParameter(kEFFECT_BYPASS_ID),
                  *p.getParamState().getParameter(kDIST_PRE_GAIN_ID),
                  *p.getParamState().getParameter(kDIST_POST_GAIN_ID),
                  *p.getParamState().getParameter(kDIST_LOWCUT_FREQ_ID),
                  *p.getParamState().getParameter(kDIST_HIGHCUT_FREQ_ID),
                  *p.getParamState().getParameter(kEFFECT_MIX_ID),
                  [this](){
        chainViewModel.deleteBlockAtIndex(selectedIndex);
            findAndNewIndex();
        }),
        chainView(chainViewModel, [this](int i) { clickedAtBlock(i); } ),
        newBlockParamView(chainViewModel)
    
{
    setSize (1000, 600);
        
    newBlockParamView.setCreateDistAction([this](std::string distID) {
        if (chainViewModel.createDistBlock(distID, selectedIndex))
            updateSelectedIndex(selectedIndex);
    });
    newBlockParamView.setCreateIRAction([this](std::string irID, File irFile) {
        if (chainViewModel.createIRBlock(irID, selectedIndex, irFile))
            updateSelectedIndex(selectedIndex);
    });
    
    
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    
    inGainLabel.setFont(15);
    inGainLabel.setText("Input\nGain", dontSendNotification);
    inGainLabel.attachToComponent(&inGainSlider, false);
    inGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    inGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 30);
    addAndMakeVisible(inGainLabel);
    addAndMakeVisible(inGainSlider);
    
    outGainLabel.setFont(15);
    outGainLabel.setText("Output\nGain", dontSendNotification);
    outGainLabel.attachToComponent(&outGainSlider, false);
    outGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 30);
    addAndMakeVisible(outGainLabel);
    addAndMakeVisible(outGainSlider);
    
    addAndMakeVisible(chainView);
    addAndMakeVisible(editPanel);
    
    addChildComponent(newBlockParamView);
    
    startTimerHz(24);
    
    findAndNewIndex();
}

DistBlockAudioProcessorEditor::~DistBlockAudioProcessorEditor()
{
}


//==============================================================================
void DistBlockAudioProcessorEditor::timerCallback()
{
    inputMeter.repaint();
    outputMeter.repaint();
//    chainView.repaint();
}

//==============================================================================
void DistBlockAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::darkgrey);
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void DistBlockAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    const auto bounds = getLocalBounds().toFloat();
    
    const auto inset = 10.f;
    const auto margin = 5.f;
    
    const auto meterH = bounds.getHeight() - (2*inset);
    const auto meterY = (bounds.getHeight() - meterH) / 2;
    const auto meterW = 20.f;

    inputMeter.setBounds(inset, meterY, meterW, meterH);
    inGainSlider.setBounds(inputMeter.getBounds().getRight() + margin, meterY + 40, 50, meterH - 40);
    outputMeter.setBounds(bounds.getWidth() - meterW - inset, meterY, meterW, meterH);
    outGainSlider.setBounds(outputMeter.getBounds().getX() - 55 - margin, meterY + 40, 55, meterH - 40);
    
    const auto chainY = 20.f;
    const auto chainX = inGainSlider.getRight() + margin;
    const auto chainW = outGainSlider.getX() - margin - chainX;
    const auto chainH = chainW / 10.f;
    chainView.setBounds(chainX, chainY, chainW, chainH);
    
    const auto editX = chainX;
    const auto editW = chainW;
    const auto editY = chainView.getBottom() + margin;
    const auto editH = bounds.getHeight() - editY - inset;
    editPanel.setBounds(editX, editY, editW, editH);
    
    newBlockParamView.setBounds(editPanel.getBounds());
}

void DistBlockAudioProcessorEditor::clickedAtBlock(int index)
{
//    printf("%d\n", index);
    updateSelectedIndex(index);
}

void DistBlockAudioProcessorEditor::updateSelectedIndex(int index)
{
    if (index < 0) {
        return;
    }
    
//    if (selectedIndex == index) return;
    selectedIndex = index;
    
    auto type = chainViewModel.getBlockType(index);
    newBlockParamView.setVisible(type == Gui::IDLE);
    editPanel.setVisible(type != Gui::IDLE);
    editPanel.setType(type);
    
    audioProcessor.setSelectedBlockIndex(index);
    chainView.setSelectedIndex(index);
}

void DistBlockAudioProcessorEditor::findAndNewIndex()
{
    auto index = chainViewModel.getTheFirstNotEmptyBlockIndex();
    updateSelectedIndex((index >= 0 ? index : 0));
}
