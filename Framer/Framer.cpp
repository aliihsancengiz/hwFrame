#include "Framer.hpp"

namespace frame_specification {

uint8_t FrameSpecification::SofIndicator = 0x12;
uint8_t FrameSpecification::EofIndicator = 0x13;
uint8_t FrameSpecification::ESCIndicator = 0x14;
}  // namespace frame_specification
namespace framer {

Framer::Framer()
{
    event_bus::EventBus::getInstance().registerEvent<frame_events::GotEncodedFrame>(
      std::bind(&Framer::onEncodedFrame, this, std::placeholders::_1));

    event_bus::EventBus::getInstance().registerEvent<frame_events::GotDecodedFrame>(
      std::bind(&Framer::onDecodedFrame, this, std::placeholders::_1));
}

void Framer::onDecodedFrame(frame_events::GotDecodedFrame frame) {}
void Framer::onEncodedFrame(frame_events::GotEncodedFrame frame) {}

void Framer::pushData(frameType frame, PushType type)
{
    if (type == PushType::ENCODER) {
        encoder.pushEncoder(frame.data(), frame.size());
    } else if (type == PushType::DECODER) {
        decoder.pushToDecoder(frame.data(), frame.size());
    }
}

}  // namespace framer
