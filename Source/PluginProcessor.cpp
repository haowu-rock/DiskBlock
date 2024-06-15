/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
DistBlockAudioProcessor::DistBlockAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        vts(*this, nullptr, "PARAMS", createParameterLayout())
#endif
{
    globalInGain = vts.getRawParameterValue(kDIST_GLOBAL_IN_GAIN_ID);
    globalOutGain = vts.getRawParameterValue(kDIST_GLOBAL_OUT_GAIN_ID);
    preGain = vts.getRawParameterValue(kDIST_PRE_GAIN_ID);
    postGain = vts.getRawParameterValue(kDIST_POST_GAIN_ID);
    bypass = vts.getRawParameterValue(kEFFECT_BYPASS_ID);
    mixing = vts.getRawParameterValue(kEFFECT_MIX_ID);
    distLowCutFreq = vts.getRawParameterValue(kDIST_LOWCUT_FREQ_ID);
    distHighCutFreq = vts.getRawParameterValue(kDIST_HIGHCUT_FREQ_ID);
    
    inGainProcesser.setGainDecibels (0.f);
    outGainProcesser.setGainDecibels (0.f);
}

DistBlockAudioProcessor::~DistBlockAudioProcessor()
{
    releaseResources();
}

//==============================================================================
const juce::String DistBlockAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistBlockAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistBlockAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistBlockAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistBlockAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistBlockAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DistBlockAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistBlockAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistBlockAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistBlockAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistBlockAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    inputLevelDB.reset(sampleRate, 0.2);
    outputLevelDB.reset(sampleRate, 0.2);
    
    inputLevelDB.setCurrentAndTargetValue(-100);
    outputLevelDB.setCurrentAndTargetValue(-100);
    
    flowManager.prepare(sampleRate, samplesPerBlock);
    
    const dsp::ProcessSpec spec{sampleRate, static_cast<uint32>(samplesPerBlock), 1};
    inGainProcesser.prepare(spec);
    outGainProcesser.prepare(spec);
}

void DistBlockAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    flowManager.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistBlockAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DistBlockAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    inputLevelDB.skip(buffer.getNumSamples());
    outputLevelDB.skip(buffer.getNumSamples());
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    if (totalNumInputChannels < 1) {
        for (auto i = 0; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());
        return;
    }
    
    float *samples = buffer.getWritePointer(0);
    
    // apply input gain
    juce::dsp::AudioBlock<float> audioBlock(&samples, 1, buffer.getNumSamples());
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    inGainProcesser.setGainDecibels(globalInGain->load());
    inGainProcesser.process(context);
    
    const auto inputDB = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (inputDB < inputLevelDB.getCurrentValue())
        inputLevelDB.setTargetValue(inputDB);
    else
        inputLevelDB.setCurrentAndTargetValue(inputDB); // no smooth
    
    // update parameter
    if (curWaveShaperBlock)
    {
        curWaveShaperBlock->setBypass(bypass->load());
        curWaveShaperBlock->setPreGainDB(preGain->load());
        curWaveShaperBlock->setPostGainDB(postGain->load());
        curWaveShaperBlock->setMix(mixing->load());
        curWaveShaperBlock->setLowcutFreq(distLowCutFreq->load());
        curWaveShaperBlock->setHighcutFreq(distHighCutFreq->load());
    }
    else if (curIRBlock)
    {
        curIRBlock->setBypass(bypass->load());
        curIRBlock->setMix(mixing->load());
    }
    
    // Process
    flowManager.process(buffer.getWritePointer(0), buffer.getNumSamples());
    
    // apply output gain
    outGainProcesser.setGainDecibels(globalOutGain->load());
    outGainProcesser.process(context);
    
    for (auto i = 2; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Copy channel
    if (totalNumOutputChannels >= 2) {
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    }
    
    const auto outputDB = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (outputDB < outputLevelDB.getCurrentValue())
        outputLevelDB.setTargetValue(outputDB);
    else
        outputLevelDB.setCurrentAndTargetValue(outputDB); // no smooth
    
//    // In case we have more outputs than inputs, this code clears any output
//    // channels that didn't contain input data, (because these aren't
//    // guaranteed to be empty - they may contain garbage).
//    // This is here to avoid people getting screaming feedback
//    // when they first compile a plugin, but obviously you don't need to keep
//    // this code if your algorithm always overwrites all the output channels.
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
//        buffer.clear (i, 0, buffer.getNumSamples());
//
//    // This is the place where you'd normally do the guts of your plugin's
//    // audio processing...
//    // Make sure to reset the state if your inner loop is processing
//    // the samples and the outer loop is handling the channels.
//    // Alternatively, you can process the samples with the channels
//    // interleaved by keeping the same state.
//    for (int channel = 0; channel < totalNumInputChannels; ++channel)
//    {
//        auto* channelData = buffer.getWritePointer (channel);
//        
//        // ..do something to the data...
//    }
}

//==============================================================================
bool DistBlockAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistBlockAudioProcessor::createEditor()
{
    return new DistBlockAudioProcessorEditor (*this);
}

//==============================================================================
void DistBlockAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DistBlockAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistBlockAudioProcessor();
}


//==============================================================================
AudioProcessorValueTreeState::ParameterLayout DistBlockAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    const float gain_min = -30.f;
    const float gain_max = 40.f;
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_GLOBAL_IN_GAIN_ID, 1),
                                                    "Input Gain",
                                                    juce::NormalisableRange<float>(gain_min, gain_max),
                                                    0.f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_GLOBAL_OUT_GAIN_ID, 1),
                                                    "Output Gain",
                                                    juce::NormalisableRange<float>(gain_min, gain_max),
                                                    0.f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_PRE_GAIN_ID, 1),
                                                    "Pre Gain",
                                                    juce::NormalisableRange<float>(gain_min, gain_max),
                                                    0.f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_POST_GAIN_ID, 1),
                                                    "Post Gain",
                                                    juce::NormalisableRange<float>(gain_min, gain_max),
                                                    0.f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_LOWCUT_FREQ_ID, 1),
                                                    "Low Cut Freq",
                                                    juce::NormalisableRange<float>(20, 500),
                                                    20.f));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kDIST_HIGHCUT_FREQ_ID, 1),
                                                    "High Cut Freq",
                                                    juce::NormalisableRange<float>(3000, 20000),
                                                    20000.f));
    params.push_back(std::make_unique<AudioParameterBool>(ParameterID(kEFFECT_BYPASS_ID, 1),
                                                    "Bypass",
                                                    false));
    params.push_back(std::make_unique<AudioParameterFloat>(ParameterID(kEFFECT_MIX_ID, 1),
                                                    "Mix",
                                                    juce::NormalisableRange<float>(0.f, 1.f),
                                                    1.f));
    return { params.begin(), params.end() };
}



void DistBlockAudioProcessor::setSelectedBlockIndex(int index)
{
    if (index < 0) {
        curWaveShaperBlock = nullptr;
        curIRBlock = nullptr;
        return;
    }
    
    auto type = flowManager.getBlockTypeAtIndex(index);
    if (!type) {
        return;
    }
    
    auto bypass_param = vts.getParameter(kEFFECT_BYPASS_ID);
    auto block = flowManager.getBlock(index);
    if (!block) {
        return;
    }
    
    bypass_param->beginChangeGesture();
    bypass_param->setValueNotifyingHost(block->isBypassed());
    bypass_param->endChangeGesture();
    
    auto mix_param = vts.getParameter(kEFFECT_MIX_ID);
    mix_param->beginChangeGesture();
    mix_param->setValueNotifyingHost(jmap(block->getMix(),
                                      mix_param->getNormalisableRange().start,
                                      mix_param->getNormalisableRange().end,
                                      0.f,
                                      1.f));
    mix_param->endChangeGesture();
    
    if (type == Routing::BlockType::DIST) {
        auto distBlock = flowManager.getWaveShaperBlock(index);
        curWaveShaperBlock = distBlock;
        curIRBlock = nullptr;
        if (!distBlock) return;
        
        auto pre_param = vts.getParameter(kDIST_PRE_GAIN_ID);
        auto post_param = vts.getParameter(kDIST_POST_GAIN_ID);
        auto lowcut_param = vts.getParameter(kDIST_LOWCUT_FREQ_ID);
        auto highcut_param = vts.getParameter(kDIST_HIGHCUT_FREQ_ID);
        
        pre_param->beginChangeGesture();
        pre_param->setValueNotifyingHost(jmap(distBlock->getPreGainDB(),
                                            pre_param->getNormalisableRange().start,
                                            pre_param->getNormalisableRange().end,
                                            0.f,
                                            1.f));
        pre_param->endChangeGesture();
        
        post_param->beginChangeGesture();
        post_param->setValueNotifyingHost(jmap(distBlock->getPostGainDB(),
                                            post_param->getNormalisableRange().start,
                                            post_param->getNormalisableRange().end,
                                            0.f,
                                            1.f));
        post_param->endChangeGesture();
        
        lowcut_param->beginChangeGesture();
        lowcut_param->setValueNotifyingHost(jmap(distBlock->getLowcutFreq(),
                                            lowcut_param->getNormalisableRange().start,
                                            lowcut_param->getNormalisableRange().end,
                                            0.f,
                                            1.f));
        lowcut_param->endChangeGesture();
        
        highcut_param->beginChangeGesture();
        highcut_param->setValueNotifyingHost(jmap(distBlock->getHighcutFreq(),
                                            highcut_param->getNormalisableRange().start,
                                            highcut_param->getNormalisableRange().end,
                                            0.f,
                                            1.f));
        highcut_param->endChangeGesture();
        
    }
    else if (type == Routing::BlockType::IR) {
        auto IRBlock = flowManager.geIRBlock(index);
        curIRBlock = IRBlock;
        curWaveShaperBlock = nullptr;
        if (!IRBlock) {
            return;
        }
    }
    
}
