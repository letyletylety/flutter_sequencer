/*
See LICENSE folder for this sample’s licensing information.

Abstract:
A DSPKernel subclass implementing the realtime signal processing portion of the AUv3FilterDemo audio unit.
*/
#ifndef SfizzDSPKernel_hpp
#define SfizzDSPKernel_hpp

#ifdef __cplusplus
#import "DSPKernel.hpp"
#import "sfizz.hpp"
#import <vector>
#import <iostream>

/*
 SfizzDSPKernel
 Calls Sfizz render code and handles MIDI events.
 As a non-ObjC class, this is safe to use from render thread.
 */
class SfizzDSPKernel : public DSPKernel {
public:

    // MARK: Member Functions

    SfizzDSPKernel() {}

    void init(int inChannelCount, double inSampleRate) {
        channelCount = inChannelCount;
        sampleRate = float(inSampleRate);

        mSampler = std::make_unique<sfz::Sfizz>();
        mSampler->setSampleRate(sampleRate);
        mSampler->setSamplesPerBlock(maximumFramesToRender());
    }

    void reset() {
    }

    bool isBypassed() {
        return bypassed;
    }

    void setBypass(bool shouldBypass) {
        bypassed = shouldBypass;
    }

    void setParameter(AUParameterAddress address, AUValue value) {
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            default: return 0;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }
    
    bool loadSfzFile(const std::string& sfzPathString) {
        auto loadResult = mSampler->loadSfzFile(sfzPathString);
        
        auto numRegions = mSampler->getNumRegions();
        auto numPreloadedSamples = mSampler->getNumPreloadedSamples();
        
        if (!loadResult) {
            std::cout << "SFZ load failed";
        }
        
        if (numRegions == 0 || numPreloadedSamples == 0) {
            std::cout << "No samples loaded";
            return false;
        }
        
        return loadResult;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {
        if (channelCount != 2) {
            std::cout << "Only stereo supported";
        }
        
        float* buffers[2];
        float leftBuffer[frameCount];
        float rightBuffer[frameCount];
        buffers[0] = leftBuffer;
        buffers[1] = rightBuffer;
        //buffers[0] = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset;
        //buffers[1] = (float*)outBufferListPtr->mBuffers[1].mData + bufferOffset;
        mSampler->renderBlock(buffers, frameCount, 2);

        bool hasOutput = false;
        
        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {
            if (buffers[0][frameIndex] > 0) {
                hasOutput = true;
            }
            
            ((float*)outBufferListPtr->mBuffers[0].mData)[frameIndex + bufferOffset] = buffers[0][frameIndex];
            ((float*)outBufferListPtr->mBuffers[1].mData)[frameIndex + bufferOffset] = buffers[1][frameIndex];
            
            float* out0 = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset + frameIndex;
            if (*out0 > 0) {
                hasOutput = true;
            }
        }
        
        if (!hasOutput) {
            std::cout << "No output?";
        }
        
    }
    
    void handleMIDIEvent(AUMIDIEvent const& midiEvent) override {
        if (midiEvent.eventType == 8) {
            auto midiStatus = midiEvent.data[0];
            auto midiData1 = midiEvent.data[1];
            auto midiData2 = midiEvent.data[2];
            
            if (midiStatus == 0x90) {
                mSampler->noteOn(0, midiData1, midiData2);
            } else if (midiStatus == 0x80) {
                mSampler->noteOff(0, midiData1, midiData2);
            }
        }
    }

    // MARK: Member Variables

private:
    int channelCount;
    float sampleRate = 44100.0;

    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;

    bool bypassed = false;

public:
    std::unique_ptr<sfz::Sfizz> mSampler;
};

#endif
#endif /* SfizzDSPKernel_hpp */
