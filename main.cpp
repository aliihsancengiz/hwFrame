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
    void onDecodedFrame(frameType frame) {}
    void onEncodedFrame(frameType frame) {}

  private:
    FrameEncoder encoder;
    FrameDecoder decoder;

  protected:
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
    FrameEncoder encoder;
    FrameDecoder decoder;
    uint8_t buffer[] = {0x61, 0x62, 0x12, 0x13, 0x14, 0x63, 0x64, 0x65};
    std::cout << "Actual Frame   : ";
    for (size_t i = 0; i < GetArrLength(buffer); i++) {
        std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << "\n";
    encoder.pushEncoder(buffer, GetArrLength(buffer));
    auto encodedframe = encoder.getFrame().value();

    std::cout << "Encoded Frame  : ";
    print_frame(encodedframe);

    decoder.pushToDecoder(encodedframe.data(), encodedframe.size());

    auto decodedframe = decoder.getFrame().value();

    std::cout << "Decoded Frame  : ";
    print_frame(decodedframe);

    return 0;
}
