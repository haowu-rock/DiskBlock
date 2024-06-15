/*
  ==============================================================================

    FlowChainView.h
    Created: 30 Mar 2024 4:24:52pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GuiCommon.h"
#include "../ViewModel/FlowChainViewModel.h"
//==============================================================================
/*
*/
namespace Gui {

class FlowChainItemView : public juce::Component
{
public:
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent& event) override;
    
    void set(SlotType type, bool bypass, std::string name, bool selected);
    
    void setClickAction(std::function<void()>&& clickHandler);
private:
    std::function<void()> clickAction;
    
    SlotType _type { IDLE };
    bool _bypass { false };
    std::string _name { "" };
    bool _selected { false };
};

class FlowChainView  : public juce::Component
{
public:
//    FlowChainView(std::function<SlotType(int)>&& typeSupplier);
    FlowChainView(ViewModel::FlowChainViewModel& model, std::function<void(int)>&& clickHandler);
    ~FlowChainView() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
//    ViewModel::FlowChainViewModel&  getViewModel() { return model; };
//    void mouseDown(const juce::MouseEvent& event) override;
    void setSelectedIndex(int index) { selectedIndex = index; }
private:
    std::function<void(int)> clickAction;
    
    ViewModel::FlowChainViewModel& model;
    
    static const int kItemViewCount { 10 };
    std::array<FlowChainItemView, kItemViewCount> itemViews;
    
    int selectedIndex = -1;
//    std::function<SlotType(int)> typeSupplier;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlowChainView)
};

} //namespace
