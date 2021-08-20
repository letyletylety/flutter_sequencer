#include <thread>
#include "AndroidEngine/AndroidEngine.h"
#include "Engine/SfizzSamplerInstrument.h"
#include "Engine/SoundFontInstrument.h"
#include "Utils/OptionArray.h"

std::unique_ptr<AndroidEngine> engine;

void check_engine() {
    if (engine == nullptr) {
        throw std::runtime_error("Engine is not set up. Ensure that setup_engine() is called before calling this method.");
    }
}

extern "C" {
    __attribute__((visibility("default"))) __attribute__((used))
    void setup_engine(Dart_Port sampleRateCallbackPort) {
        engine = std::make_unique<AndroidEngine>(sampleRateCallbackPort);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void destroy_engine() {
        engine.reset();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void add_track_sf2(const char* filename, bool isAsset, int32_t presetIndex, Dart_Port callbackPort) {
        check_engine();

        std::thread([=]() {
            auto sampleRate = engine->getSampleRate();
            auto channelCount = engine->getChannelCount();
            auto isStereo = channelCount > 1;

            auto sf2Instrument = new SoundFontInstrument(sampleRate, isStereo, filename, isAsset, presetIndex);
            auto trackIndex = engine->mSchedulerMixer.addTrack(sf2Instrument);

            callbackToDartInt32(callbackPort, trackIndex);
        }).detach();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void add_track_sfz(const char* filename, Dart_Port callbackPort) {
        check_engine();

        std::thread([=]() {
            auto sampleRate = engine->getSampleRate();
            auto channelCount = engine->getChannelCount();
            auto isStereo = channelCount > 1;

            auto sfzInstrument = new SfizzSamplerInstrument(sampleRate, isStereo, filename);

            if (sfzInstrument->didLoad()) {
                auto trackIndex = engine->mSchedulerMixer.addTrack(sfzInstrument);

                callbackToDartInt32(callbackPort, trackIndex);
            } else {
                callbackToDartInt32(callbackPort, -1);
            }
        }).detach();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void add_track_sfz_string(const char* sampleRoot, const char* sfzString, Dart_Port callbackPort) {
        check_engine();

        std::thread([=]() {
            auto sampleRate = engine->getSampleRate();
            auto channelCount = engine->getChannelCount();
            auto isStereo = channelCount > 1;

            auto sfzInstrument = new SfizzSamplerInstrument(sampleRate, isStereo, sampleRoot, sfzString);

            if (sfzInstrument->didLoad()) {
                auto trackIndex = engine->mSchedulerMixer.addTrack(sfzInstrument);

                callbackToDartInt32(callbackPort, trackIndex);
            } else {
                callbackToDartInt32(callbackPort, -1);
            }
        }).detach();
    }

__attribute__((visibility("default"))) __attribute__((used))
    void remove_track(track_index_t trackIndex) {
        check_engine();

        engine->mSchedulerMixer.removeTrack(trackIndex);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void reset_track(track_index_t trackIndex) {
        check_engine();

        engine->mSchedulerMixer.resetTrack(trackIndex);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    float get_track_volume(track_index_t trackIndex) {
        check_engine();

        return engine->mSchedulerMixer.getLevel(trackIndex);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    int32_t get_position() {
        check_engine();

        return engine->mSchedulerMixer.getPosition();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    uint64_t get_last_render_time_us() {
        check_engine();

        return engine->mSchedulerMixer.getLastRenderTimeUs();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    uint32_t get_buffer_available_count(track_index_t trackIndex) {
        return engine->mSchedulerMixer.getBufferAvailableCount(trackIndex);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void handle_events_now(track_index_t trackIndex, const uint8_t* eventData, int32_t eventsCount) {
        check_engine();

        SchedulerEvent events[eventsCount];

        rawEventDataToEvents(eventData, eventsCount, events);

        engine->mSchedulerMixer.handleEventsNow(trackIndex, events, eventsCount);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    int32_t schedule_events(track_index_t trackIndex, const uint8_t* eventData, int32_t eventsCount) {
        check_engine();

        SchedulerEvent events[eventsCount];

        rawEventDataToEvents(eventData, eventsCount, events);

        return engine->mSchedulerMixer.scheduleEvents(trackIndex, events, eventsCount);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void clear_events(track_index_t trackIndex, position_frame_t fromFrame) {
        check_engine();

        return engine->mSchedulerMixer.clearEvents(trackIndex, fromFrame);
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void engine_play() {
        check_engine();

        engine->play();
    }

    __attribute__((visibility("default"))) __attribute__((used))
    void engine_pause() {
        check_engine();

        engine->pause();
    }
}
