set(TARGET_NAME Framer)

add_library(${TARGET_NAME}
    Framer.cpp
)

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sml/include/boost
    ${CMAKE_SOURCE_DIR}/eventbus/EventBus
    PRIVATE
)

# target_link_libraries(${TARGET_NAME}
# PUBLIC

# PRIVATE
# )