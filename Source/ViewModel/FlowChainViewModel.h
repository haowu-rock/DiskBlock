/*
  ==============================================================================

    FlowChainViewModel.h
    Created: 3 Apr 2024 9:11:02pm
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include "../Routing/Manager.h"
#include "../GUI/GuiCommon.h"

namespace ViewModel {

class FlowChainViewModel
{
public:
    FlowChainViewModel(Routing::Manager& manager);
    
    Gui::SlotType getBlockType(const int index);
    bool getBypass(const int index);
    void setBypass(const int index, bool bypass);
    void toggleBypass(const int index);
    std::string getBlockName(const int index);
    
    /* if return -1, means all blocks are empty */
    int getTheFirstNotEmptyBlockIndex();
    
    void deleteBlockAtIndex(int i);
    
//    void create
    const std::array<std::string, 7>& getDistIDArray() { return distIDArray; };
    const std::array<std::string, 2>& getIRIDArray() { return irIDArray; };
    std::string getDistName(std::string ID) {
        auto it = Routing::dist_type_name_map.find(ID);
        if (it == Routing::dist_type_name_map.end()) { // not supported ID
            return "";
        }
        return it->second;
    }
    std::string getIRName(std::string ID) {
        auto it = Routing::ir_type_name_map.find(ID);
        if (it == Routing::ir_type_name_map.end()) { // not supported ID
            return "";
        }
        return it->second;
    }
    
    bool createDistBlock(const std::string ID, const int index) { return manager.createDistBlock(ID, index); }
    bool createIRBlock(const std::string ID, const int index, const juce::File file) { return manager.createIRBlock(ID, index, file); }
    
private:
    Routing::Manager& manager;
    
    const std::array<std::string, 7> distIDArray {
        Routing::kDIST_TANH_ID,
        Routing::kDIST_FUZZEXP_ID,
        Routing::kDIST_HARD_ID,
        Routing::kDIST_ARRAYA_ID,
        Routing::kDIST_CUBIC_ID,
        Routing::kDIST_ARCTAN_ID,
        Routing::kDIST_SOFT_ID
    };
    
    const std::array<std::string, 2> irIDArray {
        Routing::kIR_CAB_ID,
        Routing::kIR_REVERB_ID
    };
};

} // namespace
