#pragma once
#include "FrameDecoderSm.h"
#include "FrameEncoderSm.h"

namespace framer {

enum class PushType
{
    ENCODER,
    DECODER
};

struct Framer
{
    Framer();

  private:
    FrameEncoder encoder;
    FrameDecoder decoder;

  protected:
    virtual void onDecodedFrame(frame_events::GotDecodedFrame frame);
    virtual void onEncodedFrame(frame_events::GotEncodedFrame frame);
    void pushData(frameType frame, PushType type);
};

}  // namespace framer
