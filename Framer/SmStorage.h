#pragma once
#include <vector>

// Dependencies to store frames gotted from State Machines
using frameType = std::vector<std::uint8_t>;
namespace sm_storage_dep {
struct SmStorageDependency
{
    frameType mdataQueue;
    std::vector<frameType> mframeQueue;
};
}  // namespace sm_storage_dep
