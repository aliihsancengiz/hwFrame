#pragma once
#include "Events.h"
#include "FrameSpecification.h"
#include "SmStorage.h"
#include "sml.hpp"

#include <iostream>
#include <optional>

struct frameEncoderSm
{
    auto operator()() const
    {
        static constexpr auto sof_handler = [](const auto& event,
                                               sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.clear();
            dc.mdataQueue.push_back(event.ch);
        };
        static auto eof_handler = [](const auto& event, sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.push_back(event.ch);
            dc.mframeQueue.push_back(dc.mdataQueue);
            // Signal onEncodedFrame
            dc.mdataQueue.clear();
        };
        static constexpr auto appendCharacter = [](const auto& event,
                                                   sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.push_back(event.ch);
        };
        static auto appendSpecial = [this](const auto& event,
                                           sm_storage_dep::SmStorageDependency& dc) {
            dc.mdataQueue.push_back(frame_specification::FrameSpecification::ESCIndicator);
            dc.mdataQueue.push_back(event.ch);
        };
        using namespace sml;
        // clang-format off
        return make_transition_table(
            *"waiting"_s + event<frame_events::StartOfFrame> / sof_handler = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<frame_events::NormalCharacter> / appendCharacter = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<frame_events::EscCharacter> / appendSpecial = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<frame_events::EndOfFrame> / eof_handler = "waiting"_s);
        // clang-format on
    }
};

// an interface to hide state transition etc..
struct FrameEncoder
{
  private:
    sm_storage_dep::SmStorageDependency dataHolder;
    sml::sm<frameEncoderSm> encoderSm{dataHolder};

  public:
    FrameEncoder() = default;

    void pushEncoder(uint8_t buffer[], uint32_t size)
    {
        using namespace sml;
        encoderSm.process_event(
          frame_events::StartOfFrame{frame_specification::FrameSpecification::SofIndicator});
        for (size_t i = 0; i < size; i++) {
            auto currentChar = buffer[i];

            if (currentChar != frame_specification::FrameSpecification::SofIndicator &&
                currentChar != frame_specification::FrameSpecification::EofIndicator &&
                currentChar != frame_specification::FrameSpecification::ESCIndicator) {
                encoderSm.process_event(frame_events::NormalCharacter{currentChar});
            } else {
                encoderSm.process_event(frame_events::EscCharacter{currentChar});
            }
        }
        encoderSm.process_event(
          frame_events::EndOfFrame{frame_specification::FrameSpecification::EofIndicator});
    }
    std::size_t availableFrames() const
    {
        return dataHolder.mframeQueue.size();
    }
    std::optional<std::vector<uint8_t>> getFrame()
    {
        if (dataHolder.mframeQueue.size() > 0) {
            auto availableframe = dataHolder.mframeQueue.front();
            dataHolder.mframeQueue.erase(dataHolder.mframeQueue.begin());
            return availableframe;
        }
        return std::nullopt;
    }
};
