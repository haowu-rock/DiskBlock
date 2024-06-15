/*
  ==============================================================================

    Manager.cpp
    Created: 30 Mar 2024 10:50:37am
    Author:  Hao Wu

  ==============================================================================
*/

#include "Manager.h"

namespace Routing
{


Manager::Manager()
{
    noiseGate.setThreshold(-50);
    noiseGate.setRatio(5);
    noiseGate.setAttack(50);
    noiseGate.setRelease(20);
    printf("Hello Word!");
//    createIRBlock(kIR_CAB_ID, 9, File("/Users/fksky/Desktop/IR/Eminence Digital Kristian Kohle DV-77 IR Pack WAV/01 DV-77/441/01 DV-77 Linear.wav"));
//    blocks[1] = std::make_shared<WaveShaperBlock>(dist_transfer_func_map.at(kDIST_HARD_ID), dist_type_name_map.at(kDIST_HARD_ID));
//    auto ir = std::make_shared<IRBlock>("/Users/fksky/Desktop/IR/Reverb/orthodox-churchtemple-of-st.wav", "", 1024, );
//    ir->prepare(44100, 2);
//    float in[2];
//    in[0] = 1;
//    in[1] = 2;
//    float* ptr = in;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
//    in[0] = 3;
//    in[1] = 4;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
//    in[0] = 3;
//    in[1] = 2;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
//    in[0] = 1;
//    in[1] = 0;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
//    in[0] = 0;
//    in[1] = 0;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
//    in[0] = 0;
//    in[1] = 0;
//    printf("\n---------------------------------------------------------------------");
//    ir->process(ptr, 2);
}

Manager::~Manager()
{
    releaseResources();
}

void Manager::prepare(double sampleRate, int samplesPerBlock)
{
    noiseGate.prepare({sampleRate, static_cast<uint32>(samplesPerBlock), 1});
    noiseGate.reset();
    
    isRunning = true;
    FS = sampleRate;
    bufferSize = samplesPerBlock;
    
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        if (blocks[i]) {
            blocks[i]->prepare(sampleRate, samplesPerBlock);
        }
    }
}

void Manager::releaseResources()
{
    isRunning = false;
    FS = 0;
    bufferSize = 0;
}

void Manager::process(float *sampels, int numSamples)
{
    juce::dsp::AudioBlock<float> audioBlock(&sampels, 1, numSamples);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    noiseGate.process(context);
    
    for (int i = 0; i < BLOCK_COUNT; ++i) {
        std::shared_ptr<Block> block = blocks[i];
        if (block) {
            block->process(sampels, numSamples);
        }
    }
}

std::optional<BlockType> Manager::getBlockTypeAtIndex(const int index)
{
    std::shared_ptr<Block> block = blocks[index];
    if (block) {
        return block->getType();
    }
    return std::nullopt;
}

void Manager::bypassBlock(const int index, bool bypass)
{
    std::shared_ptr<Block> block = blocks[index];
    if (block) {
        block->setBypass(bypass);
    }
}

bool Manager::isBlockBypassed(const int index)
{
    std::shared_ptr<Block> block = blocks[index];
    if (block) {
        return block->isBypassed();
    }
    return false;
}

std::shared_ptr<WaveShaperBlock> Manager::getWaveShaperBlock(const int index)
{
    auto block = getBlock(index);
    if (block && (block->getType() == DIST))
    {
        return std::static_pointer_cast<WaveShaperBlock>(block);
    }
    return nullptr;
}

std::shared_ptr<IRBlock> Manager::geIRBlock(const int index)
{
    auto block = getBlock(index);
    if (block && (block->getType() == IR))
    {
        return std::static_pointer_cast<IRBlock>(block);
    }
    return nullptr;
}

void Manager::deleteBlock(const int index)
{
    auto block = getBlock(index);
    if (block) {
        blocks[index] = nullptr;
    }
}

bool Manager::createDistBlock(const std::string ID, const int index)
{
    if (blocks[index]) {
        return false;
    }
    auto it = dist_type_name_map.find(ID);
    if (it == dist_type_name_map.end()) { // not supported ID
        return false;
    }
    
    auto it_trans = dist_transfer_func_map.find(ID);
    if (it_trans == dist_transfer_func_map.end()) { // not supported ID
        return false;
    }
    auto block = std::make_shared<WaveShaperBlock>(it_trans->second, it->second);
    if (isRunning)
        block->prepare(FS, bufferSize);
    
    blocks[index] = block;
//     blocks[8] = std::make_shared<IRBlock>("/Users/fksky/Desktop/IR/Eminence Digital Kristian Kohle DV-77 IR Pack WAV/01 DV-77/441/01 DV-77 Linear.wav");
    return true;
}


bool Manager::createIRBlock(const std::string ID, const int index, const juce::File file)
{
    if (blocks[index]) {
        return false;
    }
    
    auto it = ir_type_name_map.find(ID);
    if (it == ir_type_name_map.end()) { // not supported ID
        return false;
    }
    
//    blocks[index] = std::make_shared<IRBlock>("/Users/fksky/Desktop/IR/Reverb/the-knights-hall-ir.wav", it->second);
    std::shared_ptr<IRBlock> block;
    if (ID == kIR_CAB_ID) {
        block = std::make_shared<IRBlock>(file, it->second, 1024, 1.0);
    } else {
        block = std::make_shared<IRBlock>(file, it->second, 8192, 0.2);
    }
    if (isRunning)
        block->prepare(FS, bufferSize);
    
    blocks[index] = block;
    
    
    return true;
}

} // namespace
