#ifndef __FRAME_ENCODER_H__
#define __FRAME_ENCODER_H__

#include "boost/sml.hpp"
#include <iostream>
#include <optional>

#include "FrameSpecification.h"
#include "Events.h"
#include "SmStorage.h"

// additional Event for encoder State machine
struct SpecialCharacter{uint8_t ch{};};


struct frameEncoderSm
{ 
    auto operator()()const
    {
        static constexpr auto sof_handler=[](const auto& event,SmStorageDependency& dc)
        {
            dc.mdataQueue.clear();
            dc.mdataQueue.push_back(event.ch);
        };
        static  auto eof_handler=[](const auto& event,SmStorageDependency& dc)
        {
            dc.mdataQueue.push_back(event.ch);
            dc.mframeQueue.push_back(dc.mdataQueue);
            dc.mdataQueue.clear();

        };
        static constexpr auto appendCharacter=[](const auto& event,SmStorageDependency& dc)
        {
            dc.mdataQueue.push_back(event.ch);
        };
        static  auto appendSpecial=[this](const auto& event,SmStorageDependency& dc)
        {
            dc.mdataQueue.push_back(0x14);
            dc.mdataQueue.push_back(event.ch);
        };
        using namespace sml;
        return make_transition_table(
            *"waiting"_s           + event<StartOfFrame>       / sof_handler     = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<NormalCharacter>    / appendCharacter = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<SpecialCharacter>   / appendSpecial   = "onFrameConstructor"_s,
            "onFrameConstructor"_s + event<EndOfFrame>         / eof_handler    = "waiting"_s
        );
    }
};

// an interface to hide state transition etc..
struct FrameEncoder
{
    private:
        SmStorageDependency dataHolder;
        sml::sm<frameEncoderSm> encoderSm{dataHolder};
        FrameSpecification fs;
    public:
        FrameEncoder()=default;
        FrameEncoder(FrameSpecification& _fs):fs(_fs)
        {

        }
        void pushEncoder(uint8_t buffer[],uint32_t size)
        {
            using namespace sml;
            encoderSm.process_event(StartOfFrame{fs.SofIndicator});
            for (size_t i = 0; i < size; i++)
            {
                auto currentChar=buffer[i];
                if(currentChar==fs.SofIndicator)
                    encoderSm.process_event(SpecialCharacter{currentChar});
                else if(currentChar==fs.EofIndicator)
                    encoderSm.process_event(SpecialCharacter{currentChar});
                else if(currentChar==fs.ESCIndicator)
                    encoderSm.process_event(SpecialCharacter{currentChar});
                else
                    encoderSm.process_event(NormalCharacter{currentChar});
                
            }
            encoderSm.process_event(EndOfFrame{fs.EofIndicator});
        
        }
        std::size_t availableFrames()const
        {
            return dataHolder.mframeQueue.size();
        }
        std::optional<std::vector<uint8_t>> getFrame()
        {
            if (dataHolder.mframeQueue.size()>0)
            {
                auto availableframe= dataHolder.mframeQueue.front();
                dataHolder.mframeQueue.erase(dataHolder.mframeQueue.begin());
                return availableframe;
            }
            return std::nullopt;
        }

};

#endif
