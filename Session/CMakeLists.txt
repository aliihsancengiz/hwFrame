set(TARGET_NAME Session)

add_library(${TARGET_NAME}
    Session.cpp
)

target_include_directories(${TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sml/include/boost
    ${CMAKE_SOURCE_DIR}/eventbus/EventBus
    ${CMAKE_SOURCE_DIR}/eventbus/Framer
    PRIVATE
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    Framer
    Boost::system
    Boost::exception
PRIVATE
)