/*
  ==============================================================================

    Block.h
    Created: 30 Mar 2024 10:50:44am
    Author:  Hao Wu

  ==============================================================================
*/

#pragma once

#include <atomic>
#include <string>

namespace Routing
{


enum BlockType {
 DIST,
 IR
};

class Block
{
public:
    Block(BlockType tp, std::string n, float defaultMix): type(tp), name(n), mix(defaultMix) { };
    
    /*
     In C++, when you delete an object through a pointer to a base class, the base class's destructor is called. If the base class's destructor is not virtual, this means only the base class's destructor is executed, not the derived class's destructor. This can lead to resource leaks if the derived class has acquired resources that need to be released in its destructor.
     Marking the base class's destructor as virtual ensures that the destructor of the derived class is called first, followed by the base class's destructor. This is essential for proper cleanup of resources, especially in a hierarchy of classes.
     */
    virtual ~Block() { };
   
    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    virtual void releaseResources() = 0;
    virtual void process(float *sampels, int numSamples) = 0;
    
    BlockType getType() { return type; }
    
    void setBypass(const bool bypass) { this->bypass = bypass; }
    bool isBypassed() { return bypass; }
    
    void setMix(float m) { mix = m; }
    float getMix() { return mix; }
    
    std::string getName() { return name; }
protected:
    BlockType type;
    std::string name;
    std::atomic<bool> bypass { false };
    float mix;
    
};


} // namespace

