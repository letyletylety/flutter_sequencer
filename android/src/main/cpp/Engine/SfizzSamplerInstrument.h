#ifndef SFIZZ_SAMPLER_INSTRUMENT_H
#define SFIZZ_SAMPLER_INSTRUMENT_H

#include "IInstrument.h"
#include "../Utils/AssetManager.h"
#include "sfizz.hpp"

class SfizzSamplerInstrument : public IInstrument {
public:
    SfizzSamplerInstrument(int32_t sampleRate, bool isStereo, const char* path, bool isAsset) {
        assert(isStereo == true); // Only stereo is supported

        mSampler = std::make_unique<sfz::Sfizz>();
        mSampler->setSampleRate(sampleRate);
        mSampler->setSamplesPerBlock(1024);
        std::string pathStr(path);
        bool didLoad;

        if (isAsset) {
            auto asset = openAssetBuffer(path);
            auto assetBuffer = static_cast<const char*>(AAsset_getBuffer(asset));
            auto assetLength = AAsset_getLength(asset);

            std::string sfzString(assetBuffer, assetLength);

            didLoad = mSampler->loadSfzString(pathStr, sfzString);

            AAsset_close(asset);
        } else {
            didLoad = mSampler->loadSfzFile(pathStr);
        }

        auto numRegions = mSampler->getNumRegions();
        auto numPreloadedSamples = mSampler->getNumPreloadedSamples();

        LOGI("SFZ did load: %i, num regions: %i, num preloaded: %i", didLoad, numRegions, numPreloadedSamples);
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

        mSampler->renderBlock(buffers, numFrames, 1);

        for (int f = 0; f < numFrames; f++) {
            for (int c = 0; c < 2; c++) {
                audioData[f * 2 + c] = buffers[c][f];
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

private:
    std::unique_ptr<sfz::Sfizz> mSampler;
};

#endif //SFIZZ_SAMPLER_INSTRUMENT_H
