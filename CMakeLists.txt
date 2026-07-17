cmake_minimum_required(VERSION 3.16)

project(Parrot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(Parrot
    src/main.cpp
    src/data/abstractrepository.h
    src/data/databasemanager.cpp
    src/data/songrepository.h
    src/data/databasemanager.h
    src/data/songrepository.cpp
    src/data/domain.cpp
    src/data/domain.h
    src/data/playlistrepository.h
    src/data/playlistrepository.cpp
    src/data/albumrepository.h
    src/data/albumrepository.cpp
    src/data/accountrepository.h
    src/data/listenerrepository.h
    src/data/listenerrepository.cpp
    src/data/artistrepository.h
    src/data/artistrepository.cpp
)

set_target_properties(Parrot PROPERTIES
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(Parrot
    PRIVATE Qt6::Widgets
            Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS Parrot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(Parrot
    PRIVATE src/
)
