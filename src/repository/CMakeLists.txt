find_package(Qt6 REQUIRED COMPONENTS Sql)

qt_add_library(Repository STATIC
    accountrepository.cpp
    abstractrepository.h
    databasemanager.cpp
    songrepository.h
    databasemanager.h
    songrepository.cpp
    playlistrepository.h
    playlistrepository.cpp
    albumrepository.h
    albumrepository.cpp
    accountrepository.h
    listenerrepository.h
    listenerrepository.cpp
    artistrepository.h
    artistrepository.cpp
)

target_include_directories(Repository
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Repository
    PUBLIC
    Domain
    Qt6::Sql
)
