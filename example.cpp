#include "miniaudio.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>


ma_result seek_audio(ma_decoder *pDecoder, ma_uint64 frameIndex) {
    return ma_decoder_seek_to_pcm_frame(pDecoder, frameIndex);
}
void update_gauge(ftxui::Component& gauge, float value) {
    gauge->Set(value);
}

int main() {
    ma_decoder decoder;
    ma_result result;

    // Initialize decoder with an example audio file
    result = ma_decoder_init_file("/storage/3461-6461/Music/Radio 001_sd.mp3", NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize decoder.\n");
        return -1;
    }

    // Create a gauge component
    auto gauge = ftxui::Gauge(0.0f);

    // Seek to a specific frame and update the gauge
    ma_uint64 frameIndex = 1000;
    result = seek_audio(&decoder, frameIndex);
    if (result == MA_SUCCESS) {
        // Update gauge value based on the audio seek position
        float progress = static_cast<float>(frameIndex) / static_cast<float>(decoder.outputFrameCount);
        update_gauge(gauge, progress);
    } else {
        printf("Failed to seek to frame.\n");
    }

    // Uninitialize the decoder
    ma_decoder_uninit(&decoder);

    // Display the gauge
    auto screen = ftxui::ScreenInteractive::FitComponent();
    screen.Loop(ftxui::Renderer(gauge));

    return 0;
}
