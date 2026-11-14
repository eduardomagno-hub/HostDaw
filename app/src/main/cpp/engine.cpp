#include <jni.h>
#include <oboe/Oboe.h>
#include <atomic>
#include <cmath>
#include <memory>

class SineEngine : public oboe::AudioStreamDataCallback {
public:
    bool start() {
        oboe::AudioStreamBuilder b;
        b.setDirection(oboe::Direction::Output)
         ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
         ->setSharingMode(oboe::SharingMode::Exclusive)
         ->setFormat(oboe::AudioFormat::Float)
         ->setChannelCount(oboe::ChannelCount::Stereo)
         ->setDataCallback(this);
        if (b.openStream(stream_) != oboe::Result::OK) return false;
        sampleRate_ = stream_->getSampleRate();
        return stream_->requestStart() == oboe::Result::OK;
    }

    void stop() {
        if (stream_) { stream_->stop(); stream_->close(); stream_.reset(); }
    }

    void setFreq(float f) { freq_.store(f); }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream*, void* data,
                                          int32_t numFrames) override {
        auto* out = static_cast<float*>(data);
        const double inc = 2.0 * M_PI * freq_.load() / sampleRate_;
        for (int i = 0; i < numFrames; ++i) {
            float v = static_cast<float>(std::sin(phase_)) * 0.2f;
            out[2 * i] = v;
            out[2 * i + 1] = v;
            phase_ += inc;
            if (phase_ > 2.0 * M_PI) phase_ -= 2.0 * M_PI;
        }
        return oboe::DataCallbackResult::Continue;
    }

private:
    std::shared_ptr<oboe::AudioStream> stream_;
    std::atomic<float> freq_{440.0f};
    double phase_ = 0.0;
    double sampleRate_ = 48000.0;
};

static SineEngine engine;

extern "C" {
JNIEXPORT jboolean JNICALL
Java_com_example_hostdaw_MainActivity_nativeStart(JNIEnv*, jobject) { return engine.start(); }

JNIEXPORT void JNICALL
Java_com_example_hostdaw_MainActivity_nativeStop(JNIEnv*, jobject) { engine.stop(); }

JNIEXPORT void JNICALL
Java_com_example_hostdaw_MainActivity_nativeSetFreq(JNIEnv*, jobject, jfloat f) {
    engine.setFreq(f);
}
}
