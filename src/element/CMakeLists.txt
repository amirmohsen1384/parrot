find_package(Qt6 REQUIRED COMPONENTS Multimedia Gui)

qt_add_library(Element STATIC
    musicplayer.h
    photoviewer.h
    musicplayer.cpp
    photoviewer.cpp
)

target_include_directories(Element
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Element
    PUBLIC
    Domain
    Qt6::Widgets
    Qt6::Multimedia
)

if(WIN32)
    target_sources(Element
        PRIVATE
            windowsmediacontrols.h
            windowsmediacontrols.cpp
    )
    target_link_libraries(Element
        PUBLIC
            windowsapp
            runtimeobject
    )
endif()
