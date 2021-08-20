#ifndef SFIZZ_SAMPLER_INSTRUMENT_H
#define SFIZZ_SAMPLER_INSTRUMENT_H

#include "IInstrument.h"
#include "../Utils/AssetManager.h"
#include "sfizz.hpp"

class SfizzSamplerInstrument : public IInstrument {
public:
    SfizzSamplerInstrument(int32_t sampleRate, bool isStereo, const char* sampleRoot, const char* sfzString) {
        mIsStereo = isStereo;

        mSampler = std::make_unique<sfz::Sfizz>();
        mSampler->setSampleRate(sampleRate);
        mSampler->setSamplesPerBlock(1024);

        auto loadResult = mSampler->loadSfzString(sampleRoot, sfzString);
        mDidLoad = loadResult && mSampler->getNumRegions() > 0 && mSampler->getNumPreloadedSamples();
    }

    SfizzSamplerInstrument(int32_t sampleRate, bool isStereo, const char* path) {
        mIsStereo = isStereo;

        mSampler = std::make_unique<sfz::Sfizz>();
        mSampler->setSampleRate(sampleRate);
        mSampler->setSamplesPerBlock(1024);

        auto loadResult = mSampler->loadSfzFile(path);
        mDidLoad = loadResult && mSampler->getNumRegions() > 0 && mSampler->getNumPreloadedSamples();
    }

    ~SfizzSamplerInstrument() {

    }

    void renderAudio(float *audioData, int32_t numFrames) override {
        float leftBuffer[numFrames];
        float rightBuffer[numFrames];
        float* buffers[2];

        buffers[0] = leftBuffer;
        buffers[1] = rightBuffer;

        for (int f = 0; f < numFrames; f++) {
            leftBuffer[f] = 0.;
            rightBuffer[f] = 0.;
        }

        mSampler->renderBlock(buffers, numFrames);

        for (int f = 0; f < numFrames; f++) {
            if (mIsStereo) {
                for (int c = 0; c < 2; c++) {
                    audioData[f * 2 + c] = buffers[c][f];
                }
            } else {
                audioData[f] = (buffers[0][f] + buffers[1][f]) / 2;
            }
        }
    }

    void handleMidiEvent(uint8_t status, uint8_t data1, uint8_t data2) override {
        if (status == 0x90) {
            // Note On
            mSampler->noteOn(0, data1, data2);
        } else if (status == 0x80) {
            // Note Off
            mSampler->noteOff(0, data1, data2);
        }
    }

    void reset() override {
    }

    bool didLoad() {
        return mDidLoad;
    }

private:
    bool mDidLoad;
    bool mIsStereo;
    std::unique_ptr<sfz::Sfizz> mSampler;
};

#endif //SFIZZ_SAMPLER_INSTRUMENT_H
