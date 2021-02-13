#ifndef __FRAME_DECODER_H__
#define __FRAME_DECODER_H__

#include "boost/sml.hpp"
#include <iostream>
#include <optional>

#include "FrameSpecification.h"
#include "Events.h"
#include "SmStorage.h"

namespace sml=boost::sml;

// an utility to get size of static arrays
template<typename T,std::size_t Size>
static constexpr std::size_t GetArrLength(T(&)[Size]) {return Size;}




// Function Object that we pass Boost.SML to make our State Machine
struct frameDecoderSm
{
    auto operator()() const
    {
        // Actions
        static constexpr auto sof_handler=[](SmStorageDependency& dc){dc.mdataQueue.clear();};
        static constexpr auto eof_handler=[](SmStorageDependency& dc){dc.mframeQueue.push_back(dc.mdataQueue);dc.mdataQueue.clear();};
        static constexpr auto esc_handler=[](){};
        static constexpr auto normal_handler=[](const auto& event,SmStorageDependency& dc){dc.mdataQueue.push_back(event.ch);};
        using namespace sml;
        return make_transition_table(
                *"waiting"_s        + event<StartOfFrame>       / sof_handler    = "onFrameNormal"_s,
                "onFrameNormal"_s   + event<EscCharacter>       / esc_handler    = "onFrameSpecial"_s,
                "onFrameNormal"_s   + event<NormalCharacter>    / normal_handler = "onFrameNormal"_s,
                "onFrameNormal"_s   + event<EndOfFrame>         / eof_handler    = "waiting"_s,
                "onFrameSpecial"_s  + event<StartOfFrame>       / normal_handler = "onFrameNormal"_s,
                "onFrameSpecial"_s  + event<EndOfFrame>         / normal_handler = "onFrameNormal"_s,
                "onFrameSpecial"_s  + event<EscCharacter>       / normal_handler = "onFrameNormal"_s,
                "onFrameSpecial"_s  + event<NormalCharacter>    / normal_handler = "onFrameNormal"_s
                );
    }
};


// Object that will give us decoded Frames
struct FrameDecoder
{
    private:
        SmStorageDependency frameHolder;
        sml::sm<frameDecoderSm> fsm{frameHolder};
        FrameSpecification fs;
    public:
        FrameDecoder()=default;
        FrameDecoder(FrameSpecification& _fs):fs(_fs)
        {

        }
        void pushToDecoder(uint8_t currentChar)
        {
            using namespace sml;
            if(currentChar==fs.SofIndicator)
                fsm.process_event(StartOfFrame{currentChar});
            else if(currentChar==fs.EofIndicator)
                fsm.process_event(EndOfFrame{currentChar});
            else if(currentChar==fs.ESCIndicator)
                fsm.process_event(EscCharacter{currentChar});
            else
                fsm.process_event(NormalCharacter{currentChar});
        }
        size_t availableFrames()const
        {
            return frameHolder.mframeQueue.size();
        }
        std::optional<std::vector<uint8_t>> getFrame()
        {
            if (frameHolder.mframeQueue.size()>0)
            {
                auto availableframe= frameHolder.mframeQueue.front();
                frameHolder.mframeQueue.erase(frameHolder.mframeQueue.begin());
                return availableframe;
            }
            return std::nullopt;
        }

        
};



#endif
