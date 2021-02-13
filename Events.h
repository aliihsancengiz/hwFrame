#ifndef _COMMON_EVENTS_H_  
#define _COMMON_EVENTS_H_

// Common Events used in Encoder and Decoder State Machine to triger state transition
struct StartOfFrame {uint8_t ch{};};
struct EndOfFrame{uint8_t ch{};};
struct EscCharacter{uint8_t ch{};};
struct NormalCharacter{uint8_t ch{};};



#endif
