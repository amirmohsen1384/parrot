find_package(Qt6 REQUIRED COMPONENTS Multimedia Gui)

qt_add_library(Elements STATIC
    musicplayer.h
    photoviewer.h
    musicplayer.cpp
    photoviewer.cpp
)

target_include_directories(Elements
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Elements
    PUBLIC
    Domain
    Qt6::Widgets
    Qt6::Multimedia
)

if(WIN32)
    target_sources(Elements
        PRIVATE
            windowsmediacontrols.h
            windowsmediacontrols.cpp
    )
    target_link_libraries(Elements
        PUBLIC
            windowsapp
            runtimeobject
    )
endif()
