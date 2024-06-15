/*
  ==============================================================================

    NewBlockParamView.h
    Created: 15 Apr 2024 8:03:38pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../ViewModel/FlowChainViewModel.h"

//==============================================================================
/*
*/
namespace Gui
{

class NewBlockParamView  : public juce::Component
{
public:
    NewBlockParamView(ViewModel::FlowChainViewModel& viewModel);
    ~NewBlockParamView() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void setCreateDistAction(std::function<void(std::string)>&& action) { createDistAction = std::move(action); }
    void setCreateIRAction(std::function<void(std::string, File)>&& action) { createIRAction = std::move(action); }
private:
    ViewModel::FlowChainViewModel& viewModel;
    
    Label textLabel { {}, "Select Block Type:" };
    ComboBox styleMenu;
    
    Label effetLabel;
    ComboBox distMenu;
    ComboBox irMenu;

    TextButton chooseIRButton;
    Label irPathLabel;
    File irFile;
    
    TextButton createButton;
    
    std::unique_ptr<FileChooser> fileChooser;
    
    // std::string is the effect ID
    std::function<void(std::string)> createDistAction;
    std::function<void(std::string, File)> createIRAction;
    
    void styleMenuChanged();
    void distMenuChanged();
    void irMenuChanged();
    void updateVisibility();
    void chooseFile();
    void create();
    void resetAll();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewBlockParamView)
};

} // namespace
