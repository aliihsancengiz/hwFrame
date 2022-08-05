#include "Framer.hpp"

constexpr auto print_frame = [](auto& frame) {
    for (auto e : frame) {
        std::cout << std::hex << static_cast<int>(e) << " ";
    }
    std::cout << "\n";
};

struct FramedSocket : framer::Framer
{
    FramedSocket() {}

    void writeFrame(frameType f)
    {
        pushData(f, framer::PushType::ENCODER);
    }
    void receiveFrame(frameType f)
    {
        pushData(f, framer::PushType::DECODER);
    }

  protected:
    void onDecodedFrame(frame_events::GotDecodedFrame frame) override
    {
        std::cout << "Got Decoded Frame " << std::endl;
        print_frame(frame.data);
    }
    void onEncodedFrame(frame_events::GotEncodedFrame frame) override
    {
        std::cout << "Got Encoded Frame ,Send data to lower layer." << std::endl;
        print_frame(frame.data);
        pushData(frame.data, framer::PushType::DECODER);
    }
};

int main()
{

    FramedSocket s;
    frameType ff{0x61, 0x62, 0x12, 0x13, 0x14, 0x63, 0x64, 0x65};
    s.writeFrame(ff);

    return 0;
}
