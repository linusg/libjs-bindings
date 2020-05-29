cmake_minimum_required(VERSION 3.16)
project(libjs-bindings)

set(SOURCES
    src/jsb.cpp
    src/Bindings/GlobalObject.cpp
    src/Bindings/LibCore/File.cpp
    src/Bindings/LibGUI/Application.cpp
    src/Bindings/LibGUI/Notification.cpp
    src/Bindings/LibGUI/Window.cpp
)

add_executable(jsb ${SOURCES})
target_link_libraries(jsb gui gfx ipc core js line)

install(TARGETS jsb RUNTIME DESTINATION local/bin)
install(DIRECTORY examples DESTINATION local/share/jsb)
