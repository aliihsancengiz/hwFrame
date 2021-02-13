#ifndef _SM_STORAGE_H_
#define _SM_STORAGE_H_

#include <vector>

// Dependencies to store frames gotted from State Machines
struct SmStorageDependency
{
    std::vector<uint8_t> mdataQueue;
    std::vector<std::vector<uint8_t>> mframeQueue;
};


#endif
