cmake_minimum_required(VERSION 3.16)
project(SCAPMS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(QT_LIB Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_LIB Qt5::Widgets)
endif()
add_executable(SCAPMS
    src/main.cpp
    src/model/PatientRecord.cpp
    src/controller/SearchEngine.cpp
    src/controller/ClinicManager.cpp
    src/utils/FileHandler.cpp
    src/gui/MainWindow.cpp
)
target_include_directories(SCAPMS PRIVATE src)
target_link_libraries(SCAPMS PRIVATE ${QT_LIB})
