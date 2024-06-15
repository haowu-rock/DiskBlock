/*
  ==============================================================================

    VerticalGridMeter.h
    Created: 18 Mar 2024 10:45:58am
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
namespace Gui 
{

class VerticalGridMeter  : public juce::Component
{
public:
    VerticalGridMeter(std::function<float()>&& valueFunction); // && rightvalue
    ~VerticalGridMeter() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::function<float()> valueSupplier;
    
    ColourGradient gradient;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalGridMeter)
};


} // namespace
