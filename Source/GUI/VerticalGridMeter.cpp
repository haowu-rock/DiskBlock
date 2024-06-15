/*
  ==============================================================================

    VerticalGridMeter.cpp
    Created: 18 Mar 2024 10:45:58am
    Author:  Hao Wu

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VerticalGridMeter.h"

namespace Gui
{

//==============================================================================
VerticalGridMeter::VerticalGridMeter(std::function<float()>&& valueFunction)
    : valueSupplier(std::move(valueFunction))

{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

VerticalGridMeter::~VerticalGridMeter()
{
    
}


void VerticalGridMeter::paint (juce::Graphics& g)
{
    const auto level = valueSupplier();
    auto bounds = getLocalBounds().toFloat();
    
    g.setColour(Colours::black);
    g.fillRect(bounds);
    
    g.setGradientFill(gradient);
    const float minDB = -60.f;
    const float maxDB = 0.f;
    const auto scaledY = jmap(level, minDB, maxDB, 0.f, bounds.getHeight());
    
    g.fillRect(bounds.removeFromBottom(scaledY));
    
//    g.setColour(Colours::red);
//    const float lineY = (maxDB/(maxDB-minDB)) * bounds.getHeight();
//    g.drawLine(0, lineY, bounds.getWidth(), lineY);
    
}

void VerticalGridMeter::resized()
{
    auto bounds = getLocalBounds().toFloat();
    gradient = ColourGradient { Colours::green, bounds.getBottomLeft(),
                            Colours::red, bounds.getTopLeft(),
                            false};
    gradient.addColour(0.5, Colours::yellow);
}

} // namespace
