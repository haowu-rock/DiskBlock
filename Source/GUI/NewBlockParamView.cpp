/*
  ==============================================================================

    NewBlockParamView.cpp
    Created: 15 Apr 2024 8:03:38pm
    Author:  Hao Wu

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NewBlockParamView.h"

namespace Gui
{

//==============================================================================
NewBlockParamView::NewBlockParamView(ViewModel::FlowChainViewModel& viewModel)
: viewModel(viewModel)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible (textLabel);
    
    // add items to the combo-box
    addAndMakeVisible (styleMenu);
    styleMenu.addItem ("Distortion",  1);
    styleMenu.addItem ("IR Loader",   2);
    styleMenu.onChange = [this] { styleMenuChanged(); };
    styleMenu.setSelectedId (1);
    
    addAndMakeVisible(effetLabel);
    
    addAndMakeVisible (distMenu);
    auto distIDArray = viewModel.getDistIDArray();
    for (int i = 0; i < distIDArray.size(); ++i) {
        distMenu.addItem (viewModel.getDistName(distIDArray[i]),  i+1);
    }
    distMenu.onChange = [this] { distMenuChanged(); };
    distMenu.setSelectedId (1);
    
    addAndMakeVisible (irMenu);
    auto irIDArray = viewModel.getIRIDArray();
    for (int i = 0; i < irIDArray.size(); ++i) {
        irMenu.addItem (viewModel.getIRName(irIDArray[i]),  i+1);
    }
    irMenu.onChange = [this] { irMenuChanged(); };
    irMenu.setSelectedId (1);
    
    addAndMakeVisible(chooseIRButton);
    chooseIRButton.setButtonText("Choose IR File");
    chooseIRButton.setColour(TextButton::buttonColourId, Colours::blue);
    chooseIRButton.setColour(TextButton::textColourOnId, Colours::white);
    chooseIRButton.onClick = [this] {
        chooseFile();
    };
    
    addAndMakeVisible(irPathLabel);
    
    addAndMakeVisible(createButton);
    createButton.setButtonText("Create Effect Block");
    createButton.setColour(TextButton::buttonColourId, Colours::red);
    createButton.setColour(TextButton::textColourOnId, Colours::white);
    createButton.onClick = [this] {
        create();
    };
    
    // set default
    updateVisibility();
}

NewBlockParamView::~NewBlockParamView()
{
}

void NewBlockParamView::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
     draws some placeholder text to get you started.
     
     You should replace everything in this method with your own
     drawing code..
     */
    
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    
//    g.setColour (juce::Colours::grey);
//    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
//    
//    g.setColour (juce::Colours::white);
//    g.setFont (14.0f);
//    g.drawText ("NewBlockParamView", getLocalBounds(),
//                juce::Justification::centred, true);   // draw some placeholder text
}

void NewBlockParamView::resized()
{
    auto bounds = getBounds().toFloat();
    
    const float inset = 10.f;
    const float itemW = bounds.getWidth() - 2*inset;
    const float itemH = 30.f;
    
    textLabel.setBounds (inset, inset, itemW, itemH);
    styleMenu.setBounds (inset, textLabel.getBounds().getBottom() + inset, itemW, itemH);
    
    effetLabel.setBounds(inset, styleMenu.getBounds().getBottom() + inset, itemW, itemH);
    distMenu.setBounds(inset, effetLabel.getBounds().getBottom() + inset, itemW, itemH);
    irMenu.setBounds(distMenu.getBounds());
    
    chooseIRButton.setBounds(inset, irMenu.getBounds().getBottom() + inset, itemW, itemH);
    irPathLabel.setBounds(inset, chooseIRButton.getBounds().getBottom() + inset, itemW, itemH);
    
    createButton.setBounds(inset, bounds.getHeight() - inset - itemH, itemW, itemH);
}

void NewBlockParamView::styleMenuChanged()
{
    updateVisibility();
}

void NewBlockParamView::updateVisibility()
{
    const int menuID = styleMenu.getSelectedId();
    
    if (menuID == 1) // DIST
    {
        distMenu.setVisible(true);
        irMenu.setVisible(false);
        irPathLabel.setVisible(false);
        chooseIRButton.setVisible(false);
        
        effetLabel.setText("Choose Distortion Type:", dontSendNotification);
        createButton.setEnabled(true);
    }
    else if (menuID == 2) // IR
    {
        distMenu.setVisible(false);
        irMenu.setVisible(true);
        irPathLabel.setVisible(true);
        chooseIRButton.setVisible(true);
        
        effetLabel.setText("Choose IR Type:", dontSendNotification);
        
        createButton.setEnabled(irFile.exists());
    }
}

void NewBlockParamView::distMenuChanged()
{
//    const int menuID = distMenu.getSelectedId();
}

void NewBlockParamView::irMenuChanged()
{
//    const int menuID = irMenu.getSelectedId();
}

void NewBlockParamView::chooseFile()
{
    if (fileChooser != nullptr)
        return;
    
    fileChooser.reset (new FileChooser ("Select an IR wav file...", File(), "*.wav"));

    fileChooser->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles, [this](const FileChooser& fc) {
        if (fc.getURLResults().size() > 0) {
            const auto u = fc.getURLResult();
            irFile = u.getLocalFile();
            
            irPathLabel.setText("IR File: " + irFile.getFullPathName(), dontSendNotification);
            updateVisibility();
        }
        fileChooser = nullptr;
    }, nullptr);
}

void NewBlockParamView::create()
{
    const int typeMenuID = styleMenu.getSelectedId();
    if (typeMenuID == 1) // DIST
    {
        if (createDistAction) 
        {
            const int distMenuID = distMenu.getSelectedId();
            createDistAction(viewModel.getDistIDArray()[distMenuID - 1]);
            resetAll();
        }
    }
    else if (typeMenuID == 2) // IR
    {
        if (createIRAction) 
        {
            const int irMenuID = irMenu.getSelectedId();
            createIRAction(viewModel.getIRIDArray()[irMenuID - 1], irFile);
            resetAll();
        }
    }
}

void NewBlockParamView::resetAll()
{
    fileChooser = nullptr;
    irFile = juce::File();
    styleMenu.setSelectedId(1);
    distMenu.setSelectedId(1);
    irMenu.setSelectedId(1);
    irPathLabel.setText("", dontSendNotification);
}

} // namesapce
