#pragma once
// Specify custom frame layout
namespace frame_specification {
struct FrameSpecification
{
    static uint8_t SofIndicator, EofIndicator, ESCIndicator;
    FrameSpecification() = default;
};

}  // namespace frame_specification
