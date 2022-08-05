#pragma once
// Specify custom frame layout
namespace frame_specification {
struct FrameSpecification
{
    static uint8_t SofIndicator, EofIndicator, ESCIndicator;
    FrameSpecification() = default;
};
uint8_t FrameSpecification::SofIndicator = 0x12;
uint8_t FrameSpecification::EofIndicator = 0x13;
uint8_t FrameSpecification::ESCIndicator = 0x14;

}  // namespace frame_specification
