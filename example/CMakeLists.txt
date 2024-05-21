
set(SOURCES
    src/main.cpp
)

add_executable(myfirstexample ${SOURCES})

target_link_libraries(myfirstexample vsg::vsg vsgXchange::vsgXchange)

install(TARGETS myfirstexample
        RUNTIME DESTINATION bin
)


