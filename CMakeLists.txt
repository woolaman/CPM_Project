cmake_minimum_required(VERSION 3.24)

project(CPM VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)


find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


set(PROJECT_SOURCES
    main.cpp
    Mainwindow.cpp
    Mainwindow.h
    Mainwindow.ui
    LabelHistogram.h
    LabelHistogram.cpp
    LabelFloodmap.h
    LabelFloodmap.cpp
    Histogram.h
    Histogram.cpp
    Crystal.h
    Crystal.cpp
    Block.h
    Block.cpp
    Readin.h
    Readin.cpp
    Parameters.h
    PETRing.h
    PETRing.cpp
    Module.h
    Module.cpp
)

qt_add_executable(CPM
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    logo.rc
)

target_link_libraries(CPM PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    ${OpenCV_LIBS}
)

set_target_properties(CPM PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS CPM
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CPM)
endif()
