#include "FrameDecoderSm.h"
#include "FrameEncoderSm.h"
constexpr auto print_frame = [](auto& frame) {
    for (auto e : frame) {
        std::cout << std::hex << static_cast<int>(e) << " ";
    }
    std::cout << "\n";
};

enum class PushType
{
    ENCODER,
    DECODER
};

struct Framer
{
    Framer()
    {
        event_bus::EventBus::getInstance().registerEvent<frame_events::GotEncodedFrame>(
          std::bind(&Framer::onEncodedFrame, this, std::placeholders::_1));

        event_bus::EventBus::getInstance().registerEvent<frame_events::GotDecodedFrame>(
          std::bind(&Framer::onDecodedFrame, this, std::placeholders::_1));
    }

  private:
    FrameEncoder encoder;
    FrameDecoder decoder;
    void onDecodedFrame(frame_events::GotDecodedFrame frame)
    {
        std::cout << "Got Decoded Frame " << std::endl;
        print_frame(frame.data);
    }
    void onEncodedFrame(frame_events::GotEncodedFrame frame)
    {
        std::cout << "Got Encoded Frame " << std::endl;
        print_frame(frame.data);
        pushData(frame.data, PushType::DECODER);
    }

  public:
    void pushData(frameType frame, PushType type)
    {
        if (type == PushType::ENCODER) {
            encoder.pushEncoder(frame.data(), frame.size());
        } else if (type == PushType::DECODER) {
            decoder.pushToDecoder(frame.data(), frame.size());
        }
    }
};

int main()
{

    Framer f;
    frameType ff{0x61, 0x62, 0x12, 0x13, 0x14, 0x63, 0x64, 0x65};
    std::cout << "Raw Frame" << std::endl;
    print_frame(ff);
    f.pushData(ff, PushType::ENCODER);

    return 0;
}
