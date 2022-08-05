#pragma once
// Common Events used in Encoder and Decoder State Machine to triger state transition
#include <cstdint>
namespace frame_events {
struct StartOfFrame
{
    uint8_t ch{};
};
struct EndOfFrame
{
    uint8_t ch{};
};
struct EscCharacter
{
    uint8_t ch{};
};
struct NormalCharacter
{
    uint8_t ch{};
};
}  // namespace frame_events
