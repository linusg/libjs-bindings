cmake_minimum_required(VERSION 3.16)
project(libjs-bindings)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++2a -fdiagnostics-color=always -fconcepts")

set(SOURCES
    src/jsb.cpp
    src/Bindings/GlobalObject.cpp
    src/Bindings/LibCore/File.cpp
    src/Bindings/LibGUI/Application.cpp
    src/Bindings/LibGUI/Notification.cpp
    src/Bindings/LibGUI/Window.cpp
)

add_executable(jsb ${SOURCES})
target_link_libraries(jsb gui gfx ipc core js crypto line)

install(TARGETS jsb RUNTIME DESTINATION local/bin)
install(DIRECTORY examples DESTINATION local/share/jsb)
