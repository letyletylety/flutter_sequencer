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

    SfizzDSPKernel() {
        mSampler = std::make_unique<sfz::Sfizz>();
    }

    void init(int inChannelCount, double inSampleRate) {
        channelCount = inChannelCount;
        sampleRate = float(inSampleRate);

        mSampler->setSampleRate(sampleRate);
        mSampler->setSamplesPerBlock(maximumFramesToRender());
        
        auto volume = mSampler->getVolume();
        std::cout << "Volume: " << volume;
        mSampler->setVolume(1.0);
        volume = mSampler->getVolume();
        std::cout << "Volume: " << volume;
    }

    void reset() {
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
    }

    void setBuffers(AudioBufferList* inBufferList, AudioBufferList* outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }
    
    bool loadFile(const std::string& sfzPathString) {
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

    bool loadString(const std::string& sfzPathString, const std::string& sfzString) {
        auto loadResult = mSampler->loadSfzString(sfzPathString, sfzString);
        
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
        if (channelCount == 1) {
            // Mono
            float* buffers[2];
            float leftBuffer[frameCount];
            float rightBuffer[frameCount];
            
            buffers[0] = leftBuffer;
            buffers[1] = rightBuffer;

            mSampler->renderBlock(buffers, frameCount, 2);
            
            float* outBuffer = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset;
            
            for (int i = 0; i < frameCount; i++) {
                outBuffer[i] = (buffers[0][i] + buffers[1][i]) / 2;
            }
        } else {
            // Stereo
            float* buffers[2];

            buffers[0] = (float*)outBufferListPtr->mBuffers[0].mData + bufferOffset;
            buffers[1] = (float*)outBufferListPtr->mBuffers[1].mData + bufferOffset;

            mSampler->renderBlock(buffers, frameCount, 2);
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
    float sampleRate;

    AudioBufferList* inBufferListPtr = nullptr;
    AudioBufferList* outBufferListPtr = nullptr;

public:
    std::unique_ptr<sfz::Sfizz> mSampler;
};

#endif
#endif /* SfizzDSPKernel_hpp */
