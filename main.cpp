#include "FrameDecoderSm.h"
#include "FrameEncoderSm.h"



constexpr auto print_frame=[](auto& frame)
{
    for(auto e:frame)
    {
       std::cout<<std::hex<<static_cast<int>(e)<<" ";
    }
    std::cout<<"\n";
};

int main()
{
    FrameEncoder encoder;
    FrameDecoder decoder;
    uint8_t buffer[]={0x61,0x62,0x12,0x13,0x14,0x63,0x64,0x65};
    std::cout<<"Actual Frame   : ";
    for (size_t i = 0; i < GetArrLength(buffer); i++)
    {
        std::cout<<std::hex<<static_cast<int>(buffer[i])<<" ";
    }
    std::cout<<"\n";
    encoder.pushEncoder(buffer,GetArrLength(buffer));
    auto encodedframe=encoder.getFrame().value();


    std::cout<<"Encoded Frame  : ";
    print_frame(encodedframe);


    for (auto e:encodedframe)
    {
        decoder.pushToDecoder(e);
    }
    auto decodedframe=decoder.getFrame().value();
 
    std::cout<<"Decoded Frame  : ";
    print_frame(decodedframe);


    
    return 0;
}
