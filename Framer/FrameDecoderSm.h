#pragma once
#include "EventBus.hpp"
#include "Events.h"
#include "FrameSpecification.h"
#include "SmStorage.h"
#include "sml.hpp"

#include <iostream>
#include <optional>

namespace sml = boost::sml;

// an utility to get size of static arrays
template<typename T, std::size_t Size>
static constexpr std::size_t GetArrLength(T (&)[Size])
{
    return Size;
}

// Function Object that we pass Boost.SML to make our State Machine
struct frameDecoderSm
{
    auto operator()() const
    {
        // Actions
        static constexpr auto sof_handler = [](sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.clear();
        };
        static constexpr auto eof_handler = [](sm_storage_dep::SmStorageDependency& dc) {
            dc.mframeQueue.push_back(dc.mdataQueue);
            // Signal onDecodedFrame
            event_bus::EventBus::getInstance().fireEvent(
              frame_events::GotDecodedFrame{dc.mdataQueue});
            dc.mdataQueue.clear();
        };
        static constexpr auto esc_handler = []() {};
        static constexpr auto normal_handler = [](const auto& event,
                                                  sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.push_back(event.ch);
        };
        using namespace sml;
        // clang-format off
        return make_transition_table(
            *"waiting"_s + event<frame_events::StartOfFrame> / sof_handler = "onFrameNormal"_s,
            "onFrameNormal"_s + event<frame_events::EscCharacter> / esc_handler = "onFrameSpecial"_s,
            "onFrameNormal"_s + event<frame_events::NormalCharacter> / normal_handler = "onFrameNormal"_s,
            "onFrameNormal"_s + event<frame_events::EndOfFrame> / eof_handler = "waiting"_s,
            "onFrameSpecial"_s + event<frame_events::StartOfFrame> / normal_handler = "onFrameNormal"_s,
            "onFrameSpecial"_s + event<frame_events::EndOfFrame> / normal_handler = "onFrameNormal"_s,
            "onFrameSpecial"_s + event<frame_events::EscCharacter> / normal_handler = "onFrameNormal"_s,
            "onFrameSpecial"_s + event<frame_events::NormalCharacter> / normal_handler = "onFrameNormal"_s);
        // clang-format on
    }
};

// Object that will give us decoded Frames
struct FrameDecoder
{
  private:
    sm_storage_dep::SmStorageDependency frameHolder;
    sml::sm<frameDecoderSm> fsm{frameHolder};

  public:
    FrameDecoder() = default;
    void pushToDecoder(uint8_t buffer[], uint32_t size)
    {
        using namespace sml;
        for (size_t i = 0; i < size; i++) {
            auto currentChar = buffer[i];
            if (currentChar == frame_specification::FrameSpecification::SofIndicator)
                fsm.process_event(frame_events::StartOfFrame{currentChar});
            else if (currentChar == frame_specification::FrameSpecification::EofIndicator)
                fsm.process_event(frame_events::EndOfFrame{currentChar});
            else if (currentChar == frame_specification::FrameSpecification::ESCIndicator)
                fsm.process_event(frame_events::EscCharacter{currentChar});
            else
                fsm.process_event(frame_events::NormalCharacter{currentChar});
        }
    }
    size_t availableFrames() const
    {
        return frameHolder.mframeQueue.size();
    }
    std::optional<std::vector<uint8_t>> getFrame()
    {
        if (frameHolder.mframeQueue.size() > 0) {
            auto availableframe = frameHolder.mframeQueue.front();
            frameHolder.mframeQueue.erase(frameHolder.mframeQueue.begin());
            return availableframe;
        }
        return std::nullopt;
    }
};
