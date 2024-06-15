/*
  ==============================================================================

    IRBlock.cpp
    Created: 3 Apr 2024 7:13:36pm
    Author:  Hao Wu

  ==============================================================================
*/

#include "IRBlock.h"
#include <iostream>

namespace Routing {

//IRBlock::IRBlock(std::string irPath, std::string name, int irSize)
//    : IRBlock(File(irPath), name), irSize(irSize)
//{
////    // load ir file
////    formatManager.registerBasicFormats();
////    ir = (float *)calloc(irSize, sizeof(float));
////    juce::File file(irPath);
//////    printf("getSize:%lld\n", file.getSize());
////    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);
////    
////    if (reader != nullptr)
////     
////    {
////        reader->read(&ir, 1, 0, irSize);
////        delete reader;
////    }
////    
//////    for (int i = 0; i < irSize; i++)
//////    {
//////        printf("%d: %f\n", i, ir[i]);
//////    }
//////    ir = (float *)calloc(4, sizeof(float));
//////    ir[0] = 1.f;
//////    ir[1] = 1.f;
//////    ir[2] = 1.f;
//////    ir[3] = 1.f;
//}

IRBlock::IRBlock(File irFile, std::string name, int irSize, float defaultMix)
    :Block(IR, name, defaultMix), irSize(irSize)
{
    formatManager.registerBasicFormats();
    ir = (float *)calloc(irSize, sizeof(float));
//    printf("irSize:%d\n", irSize);
//    printf("getSize:%lld\n", irFile.getSize());
    juce::AudioFormatReader* reader = formatManager.createReaderFor(irFile);
    
    if (reader != nullptr)
     
    {
        reader->read(&ir, 1, 0, irSize);
        delete reader;
    }
    normalizeIR(ir, irSize);
    
//    for (int i = 0; i < irSize; i++)
//    {
//        printf("%d: %f\n", i, ir[i]);
//    }
//    ir = (float *)calloc(4, sizeof(float));
//    ir[0] = 1.f;
//    ir[1] = 1.f;
//    ir[2] = 1.f;
//    ir[3] = 1.f;
}

IRBlock::~IRBlock()
{
    std::cout << "IRBlock Destructor called for object at: " << this << std::endl;
    releaseResources();
    if (ir)
    {
        free(ir);
        ir = 0;
    }
}

void IRBlock::prepare(double sampleRate, int samplesPerBlock)
{
    printf("IR prepare: sampleRate:%f, blockSize:%d\n", sampleRate, samplesPerBlock);
    blockSize = samplesPerBlock;
    fftSize = blockSize * 2;
    numSubIR = irSize / blockSize;
    
    wetSamples = (float*)malloc(sizeof(float) * blockSize);
    
    // init
    linearOutSize = blockSize + irSize;
    linearOutput = (float *)calloc(linearOutSize, sizeof(float));
    
    // init FFT obj
    auto fftOrder = log2(fftSize);
//    printf("fftOrder:%f\n", fftOrder);
    FFT = std::make_unique<juce::dsp::FFT>(fftOrder);
    
    // ensure enough memory before processing block
    XFFTData.resize(fftSize);
    subYFFTData.resize(fftSize);
//    subIrFFTData.reserve(numSubIR);
    subIrFFTData.resize(numSubIR);
    for (int i = 0; i < numSubIR; ++i) {
        subIrFFTData[i].resize(fftSize);
    }

    //  Calculate sub H(e^jw)
    for (int i = 0; i < numSubIR; ++i) {
        for (int j = 0; j < blockSize; ++j)
            subIrFFTData[i][j] = std::complex<float>(ir[i*blockSize + j], 0.0f);
        for (int j = blockSize; j < fftSize; ++j)
            subIrFFTData[i][j] = std::complex<float>(0.0f, 0.0f);
        
//        printf("\nsub h%d:\n", i);
//        for (int j = 0; j < fftSize; ++j) {
//            printf("(%f,%f) ", subIrFFTData[i][j].real(), subIrFFTData[i][j].imag());
//        }
        FFT->perform(subIrFFTData[i].data(), subIrFFTData[i].data(), false);
        
//        printf("\nsub H%d:\n", i);
//        for (int j = 0; j < fftSize; ++j) {
//            printf("(%f,%f) ", subIrFFTData[i][j].real(), subIrFFTData[i][j].imag());
//        }
    }
}

void IRBlock::releaseResources() 
{
    blockSize = 0;
    fftSize = 0;
    numSubIR = 0;
    
    linearOutSize = 0;
    if (linearOutput) {
        free(linearOutput);
        linearOutput = 0;
    }
    if (FFT) {
        FFT.reset();
    }
    if (wetSamples) {
        free(wetSamples);
        wetSamples = 0;
    }
    XFFTData.clear();
    subYFFTData.clear();
    subIrFFTData.clear();
}

void IRBlock::process(float *sampels, int numSamples)
{
//    printf("numSample:%d\n", numSamples);
    if (bypass) {
        return;
    }
    // copy samples for processing
    memcpy(wetSamples, sampels, sizeof(float) * blockSize);
    
    // 1. Calculate X(e^jw) for once
    // Fill with x[n]
    for (int i = 0; i < numSamples; ++i)
        XFFTData[i] = std::complex<float>(wetSamples[i], 0.0f);
    // Fill remaining part with 0
    for (int i = numSamples; i < fftSize; ++i)
        XFFTData[i] = std::complex<float>(0.0f, 0.0f);
    
    // Calculate FFT for x
    FFT->perform(XFFTData.data(), XFFTData.data(), false);
    
    // 2. Calculate the convolution with each sub ir seqence by using fft
    for (int i = 0; i < numSubIR; i++)
    {
        // multiply X(e^jw) and each sub H(e^jw)
        for (int j = 0; j < fftSize; ++j) {
            subYFFTData[j] = XFFTData[j] * subIrFFTData[i][j];
        }
        
        // calculate IFFT for getting the sub y[n]
        FFT->perform(subYFFTData.data(), subYFFTData.data(), true);
        
        // Add each sub y[n] into the linearOutput cache
        for (int j = 0; j < fftSize; ++j)
            linearOutput[i*blockSize + j] += subYFFTData[j].real();
    }
    
    // 3. Take the first 'blockSize' samples from the linearOutput cache as the output for this frame
//    if (!bypass) {
        memcpy(wetSamples, linearOutput, blockSize*sizeof(float));
        for (int i = 0; i < blockSize; ++i) {
            sampels[i] = mix * wetSamples[i] + (1-mix)*sampels[i];
        }
//    }
    
    
    // 4. Move the last 'irSize' samples in the linearOut cache to the front of the cache,
    // and set the last 'blockSize' samples in the linearOut cache to 0.
    // Use memmove to handle potential overlap
    memmove(linearOutput, linearOutput + blockSize, irSize*sizeof(float));
    memset(linearOutput+irSize, 0, blockSize*sizeof(float));
    
//    printf("\noutput:\n");
//    for (int i = 0; i < numSamples; ++i) {
//        printf("%f ", sampels[i]);
//    }
}

void IRBlock::normalizeIR(float *ir, int size)
{
    const float max_amp = 0.125;
    float max = 0;
    for (int i = 0; i < size; i++)
    {
        if (ir[i] > max)
        {
            max = ir[i];
        }
    }
    
//    printf("max:%f\n", max);
    float scale = max_amp / max;
//    printf("scale:%f\n", scale);
//    printf("scaled max:%f\n", scale * max);
    
    for (int i = 0; i < size; i++)
    {
        ir[i] = ir[i] * scale;
//        printf("i:%d - :%f\n", i, ir[i]);
    }
}

//void IRBlock::process(float *sampels, int numSamples)
//{
//    // Fill with x[n]
//    for (int i = 0; i < numSamples; ++i) {
//        inputFFTData[i] = std::complex<float>(sampels[i], 0.0f);
//    }
//    printf("\ninput:\n");
//    for (int i = 0; i < numSamples; ++i) {
//        printf("%f ", inputFFTData[i].real());
//    }
//    
//    printf("\nh[n]:\n");
//    for (int i = 0; i < fftSize; ++i) {
//        printf("%f ", ir[i]);
//    }
//    
//    // append zeros
//    for (int i = numSamples; i < fftSize; ++i) {
//        inputFFTData[i] = std::complex<float>(0.0f, 0.0f);
//    }
//    
//    // Calculate X(e^jw)
//    FFT.perform(inputFFTData.data(), inputFFTData.data(), false);
//    
//    // multiply X(e^jw) and H(e^jw)
//    for (int i = 0; i < fftSize; ++i) {
//        outputFFTData[i] = inputFFTData[i] * irFFTData[i];
//    }
//    
//    // Calculate y[n]
//    FFT.perform(outputFFTData.data(), outputFFTData.data(), true);
//    
//    printf("\ny[n]:\n");
//    for (int i = 0; i < fftSize; ++i) {
//        printf("%f ", outputFFTData[i].real());
//    }
//    
//    // Skip the first (blockSize - 1) aliased samples in y[n].
//    // Fill the output channel: output = y[blockSize: 2*blockSize - 1] + overlapBuffer[0:blockSize-1]
////    if (!bypass) {
//        for (int i = 0; i < blockSize; ++i) {
////            printf("i:%d overlapBuffer:%f outputFFTData:%f\n", i, overlapBuffer[i], outputFFTData[blockSize + i].real());
//            sampels[i] = overlapBuffer[i] + outputFFTData[blockSize + i].real();
//        }
////    }
//    printf("\noutput:\n");
//    for (int i = 0; i < numSamples; ++i) {
//        printf("%f ", sampels[i]);
//    }
////    
//    // Add remaining samples starting at index blockSize in the overlap buffer
//    // with the remaining samples starting at index (2*blockSize - 1) in the y[n]
//    // store the result samples into overlap buffer
//    int overlapStartingIdx = blockSize;
//    int outputStartingIdx = 2*blockSize - 1;
//    for (int i = 0; i < overlapSize; i++) {
//        if (overlapStartingIdx + i >= overlapSize) {
//            overlapBuffer[i] = outputFFTData[outputStartingIdx + i].real();
//        } else {
//            overlapBuffer[i] = overlapBuffer[overlapStartingIdx + i] + outputFFTData[outputStartingIdx + i].real();
//        }
//    }
//    
//    printf("\noverlapbuffer:\n");
//    for (int i = 0; i < overlapSize; ++i) {
//        printf("%f ", overlapBuffer[i]);
//    }
//    
////    memcpy(overlapBuffer, overlapBuffer + numSamples, (overlapSize - numSamples) * sizeof(float));
////    // clear (overlapSize - numSamples) samples at the end of overlap buffer
////    memset(overlapBuffer + overlapSize - numSamples, 0, numSamples * sizeof(float));
////    
////    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
////    for (int i = 0; i < overlapSize; ++i) {
////        printf("%f ", overlapBuffer[i]);
////    }
////    
////    // sum num of overlapSize samples at the end of outputData and samples in overlap buffer, and store result in overlap buffer
////    for (int i = 0; i < overlapSize; ++i) {
////        overlapBuffer[i] += outputFFTData[numSamples + i].real();
////    }
////    printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
////    for (int i = 0; i < overlapSize; ++i) {
////        printf("%f ", overlapBuffer[i]);
////    }
//}

//===============================================================================



} // namespace
