/*
  ==============================================================================

    FlowChainView.cpp
    Created: 30 Mar 2024 4:24:52pm
    Author:  Hao Wu

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FlowChainView.h"

namespace Gui {

//=========================================== FlowChainItemView  =================================================
void FlowChainItemView::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    
//    const auto blockInset = 3.f;
    g.setFont (bounds.getHeight() * 0.3);
    
    const auto blockW = bounds.getWidth();
    const auto blockH = bounds.getHeight();
    
    const auto plusW = blockW * 0.5;
    const auto plusH = plusW;
    const auto plusX = (blockW - plusW) * 0.5;
    const auto plusY = plusX;
    const auto plusLineW = 3.f;
    
    Rectangle<float> rect { 0, 0, blockW,  blockH};
    
    if (_type == IDLE)
    {
        g.setColour(Colours::white);
        g.drawRoundedRectangle(rect, 5, 2);
        g.drawLine(plusX + rect.getX(), rect.getCentreY(), plusX + rect.getX() + plusW, rect.getCentreY(), plusLineW);
        g.drawLine(rect.getCentreX(), plusY + rect.getY(), rect.getCentreX(), plusY + rect.getY() + plusH, plusLineW);
    }
    else
    {
        auto textbound = rect;
        if (_type == DIST)
        {
            g.setColour (juce::Colours::darkred);
            g.drawText ("DIST", textbound.removeFromTop(rect.getHeight() * 0.45), juce::Justification::centredBottom, true);
        }
        else if (_type == IR)
        {
            g.setColour (juce::Colours::darkblue);
            g.drawText ("IR", textbound.removeFromTop(rect.getHeight() * 0.45), juce::Justification::centredBottom, true);
        }
        
        g.drawRoundedRectangle(rect, 5, 2);
//        g.drawText (_name, textbound, juce::Justification::centred, true);
        g.drawText (_name, textbound.removeFromBottom(rect.getHeight() * 0.45), juce::Justification::centredTop, true);
        if (_bypass) {
            g.setColour(Colours::grey);
            g.setOpacity(0.5);
            g.fillRect(rect);
        }
    }
    
    if (_selected) {
        g.setColour(juce::Colours::yellow);
        g.drawRoundedRectangle(rect, 5, 8);
    }
}

void FlowChainItemView::resized()
{
    
}

void FlowChainItemView::mouseDown(const juce::MouseEvent& event)
{
    if (clickAction) {
        clickAction();
    }
}

void FlowChainItemView::set(SlotType type, bool bypass, std::string name, bool selected)
{
    _type = type;
    _bypass = bypass;
    _name = name;
    _selected = selected;
}

void FlowChainItemView::setClickAction(std::function<void()>&& action)
{
    clickAction = std::move(action);
}



//================================================================================================================
//=                                              FlowChainView                                                   =
//================================================================================================================
FlowChainView::FlowChainView(ViewModel::FlowChainViewModel& model, std::function<void(int)>&& clickHandler)
    : clickAction(std::move(clickHandler)), model(model)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    for (int i = 0; i < kItemViewCount; ++i)
    {
        addAndMakeVisible(itemViews[i]);
        itemViews[i].setClickAction([&, i](){
            clickAction(i);
        });
    }
}

FlowChainView::~FlowChainView()
{
}

void FlowChainView::paint (juce::Graphics& g)
{

    const auto bounds = getLocalBounds().toFloat();
    
    const auto blockInset = 3.f;
    
    const auto blockW = bounds.getWidth() / 10.f - 2*blockInset;
    const auto blockH = blockW;

    for (int i = 0; i < 10; ++i) {
        ////        auto type = typeSupplier(i);
        const auto blockX = blockInset + (blockW + 2*blockInset) * i;
        //        Rectangle<float> rect { blockX, blockInset, blockW, blockH };
        itemViews[i].set(model.getBlockType(i),
                         model.getBypass(i),
                         model.getBlockName(i),
                         selectedIndex == i);
        itemViews[i].setBounds(blockX, blockInset, blockW, blockH);
    }

}

void FlowChainView::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


} //namespace
