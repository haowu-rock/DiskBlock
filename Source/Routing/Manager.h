/*
  ==============================================================================

    Manager.h
    Created: 30 Mar 2024 10:50:37am
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include "WaveShaperBlock.h"
#include "IRBlock.h"
#include <array>
#include <JuceHeader.h>
#include <iostream>
#include "../Clipping/TransferFunctions.h"

namespace Routing
{

// DIST TYPE ID
const std::string kDIST_TANH_ID = "DIST_TANH";
const std::string kDIST_HARD_ID = "DIST_HARD";
const std::string kDIST_FUZZEXP_ID = "DIST_FUZZEXP";
const std::string kDIST_ARRAYA_ID = "DIST_ARRAYA";
const std::string kDIST_CUBIC_ID = "DIST_CUBIC";
const std::string kDIST_ARCTAN_ID = "DIST_ARCTAN";
const std::string kDIST_SOFT_ID = "DIST_SOFT";


// IR TYPE ID
const std::string kIR_CAB_ID = "IR_CAB";
const std::string kIR_REVERB_ID = "IR_REVERB";

const std::unordered_map<std::string, std::string> dist_type_name_map = {
    { kDIST_TANH_ID, "Tanh" },
    { kDIST_HARD_ID, "Hard" },
    { kDIST_FUZZEXP_ID, "FEXP2" },
    { kDIST_ARRAYA_ID, "Arraya" },
    { kDIST_CUBIC_ID, "Cubic" },
    { kDIST_ARCTAN_ID, "Arctan" },
    { kDIST_SOFT_ID, "Soft" }
};
const std::unordered_map<std::string, std::string> ir_type_name_map = {
    { kIR_CAB_ID, "Cab" },
    { kIR_REVERB_ID, "Reverb" },
};

class Manager
{
public:
    static const int BLOCK_COUNT { 10 };
    
    Manager();
    ~Manager();
    
    void prepare(double sampleRate, int samplesPerBlock);
    
    void releaseResources();
    
    void process(float *sampels, int numSamples);
    
    std::optional<BlockType> getBlockTypeAtIndex(const int index);
    std::shared_ptr<Block> getBlockDataAtIndex(const int index) { return blocks[index]; };
    std::shared_ptr<Block> getBlock(const int index) { return blocks[index]; }
    
    // return: nullable
    std::shared_ptr<WaveShaperBlock> getWaveShaperBlock(const int index);
    std::shared_ptr<IRBlock> geIRBlock(const int index);
    
    void deleteBlock(const int index);
    
    void bypassBlock(const int index, bool bypass);
    bool isBlockBypassed(const int index);
    
    bool createDistBlock(const std::string ID, const int index);
    bool createIRBlock(const std::string ID, const int index, const juce::File file);
private:
    std::array<std::shared_ptr<Block>,BLOCK_COUNT> blocks;
    
    const std::unordered_map<std::string, float (*)(float)> dist_transfer_func_map {
        { kDIST_TANH_ID, TransferFunctions::tanh },
        { kDIST_HARD_ID, TransferFunctions::hard },
        { kDIST_FUZZEXP_ID, TransferFunctions::fuzzexp },
        { kDIST_ARRAYA_ID, TransferFunctions::arraya },
        { kDIST_CUBIC_ID, TransferFunctions::cubic },
        { kDIST_ARCTAN_ID, TransferFunctions::arctan },
        { kDIST_SOFT_ID, TransferFunctions::soft }
    };
    
    dsp::NoiseGate<float> noiseGate;
    
    std::atomic<bool> isRunning { false };
    double FS { 0 };
    int bufferSize { 0 };
};

} // namespace
