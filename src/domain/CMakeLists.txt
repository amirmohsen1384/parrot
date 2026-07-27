find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_add_library(Domain STATIC
    domain.cpp
    domain.h
)

target_include_directories(Domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Domain
    PUBLIC
    Qt6::Gui
)
