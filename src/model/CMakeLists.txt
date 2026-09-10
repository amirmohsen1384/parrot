find_package(Qt6 REQUIRED COMPONENTS Gui)

qt_add_library(Model STATIC
    genremodel.h
    genremodel.cpp
)

target_include_directories(Model
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Model
    PUBLIC
    Domain
    Qt6::Gui
)
