cmake_minimum_required(VERSION 3.16)

project(Parrot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(src/domain)

add_subdirectory(src/repository)

qt_add_executable(Parrot
    src/main.cpp
)

set_target_properties(Parrot PROPERTIES
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(Parrot
    PUBLIC
    Domain
    Repository
    Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS Parrot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(Parrot
    PRIVATE
    src/
    ${CMAKE_CURRENT_SOURCE_DIR}
)
