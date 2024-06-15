/*
  ==============================================================================

    FlowChainViewModel.cpp
    Created: 3 Apr 2024 9:11:02pm
    Author:  Hao Wu

  ==============================================================================
*/

#include "FlowChainViewModel.h"

namespace ViewModel {


FlowChainViewModel::FlowChainViewModel(Routing::Manager& manager)
    : manager(manager)
{
    
}

Gui::SlotType FlowChainViewModel::getBlockType(const int index)
{
    if (index < 0 || index >= Routing::Manager::BLOCK_COUNT) {
        return Gui::IDLE;
    }
    
    auto type = manager.getBlockTypeAtIndex(index);
    if (!type) return Gui::SlotType::IDLE;
    
    if (type == Routing::BlockType::DIST) return Gui::SlotType::DIST;
    else if (type == Routing::BlockType::IR) return Gui::SlotType::IR;
    else  return Gui::SlotType::IDLE;
}

bool FlowChainViewModel::getBypass(const int index)
{
    if (index < 0 || index >= Routing::Manager::BLOCK_COUNT) {
        return false;
    }
    
    return manager.isBlockBypassed(index);
}

void FlowChainViewModel::setBypass(const int index, bool bypass)
{
    if (index < 0 || index >= Routing::Manager::BLOCK_COUNT) {
        return;
    }
    return manager.bypassBlock(index, bypass);
}

void FlowChainViewModel::toggleBypass(const int index)
{
    if (index < 0 || index >= Routing::Manager::BLOCK_COUNT) {
        return;
    }
    setBypass(index, !getBypass(index));
}

std::string FlowChainViewModel::getBlockName(const int index)
{
    if (index < 0 || index >= Routing::Manager::BLOCK_COUNT) {
        return "";
    }
    
    auto block = manager.getBlock(index);
    if (!block) {
        return "";
    }
    return block->getName();
}

int FlowChainViewModel::getTheFirstNotEmptyBlockIndex()
{
    for (int i = 0; i < Routing::Manager::BLOCK_COUNT; ++i)
    {
        auto block = manager.getBlock(i);
        if (block) {
            return i;
        }
    }
    return -1;;
}

void FlowChainViewModel::deleteBlockAtIndex(int i)
{
    if (i < 0 || i >= Routing::Manager::BLOCK_COUNT) {
        return;
    }
    manager.deleteBlock(i);
}

} // namespace
