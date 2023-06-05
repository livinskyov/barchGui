cmake_minimum_required(VERSION 3.16)

project(BarchGui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(appBarchGui
    main.cpp
    imagelistmodel.h
    imagelistmodel.cpp
    compression.h
    compression.cpp
    watcheddirectory.h
    watcheddirectory.cpp
    threading.h
    threading.cpp
)

qt_add_qml_module(appBarchGui
    URI BarchGui
    VERSION 1.0
    QML_FILES Main.qml ImageList.qml
)

set_target_properties(appBarchGui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include_directories(../barch)

target_link_libraries(appBarchGui
    PRIVATE Qt6::Quick ../build-barch-Desktop_x86_windows_msvc2019_pe_64bit-${CMAKE_BUILD_TYPE}/barch
)

install(TARGETS appBarchGui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
