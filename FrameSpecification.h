#ifndef __FRAME_SPEC_H__
#define __FRAME_SPEC_H__


// Specify custom frame layout
struct FrameSpecification
{
    uint8_t SofIndicator=0x12,EofIndicator=0x13,ESCIndicator=0x14;
    FrameSpecification()=default;
    FrameSpecification(uint8_t sof_,uint8_t eof_,uint8_t esc_):SofIndicator(sof_),EofIndicator(eof_){}
};



#endif