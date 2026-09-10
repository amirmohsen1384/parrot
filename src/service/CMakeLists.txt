qt_add_library(Service STATIC
    accountservice.h
    accountservice.cpp
    artistservice.h
    artistservice.cpp
    listenerservice.h
    listenerservice.cpp
    playlistservice.h
    playlistservice.cpp
    albumservice.h
    albumservice.cpp
    abstractservice.h

    authenticationservice.h authenticationservice.cpp
)

target_include_directories(Service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Service
    PUBLIC
    Repository
)
