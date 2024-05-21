
set(SOURCES
    main.cpp
)

add_executable(nano ${SOURCES})

target_link_libraries(nano vsg::vsg vsgXchange::vsgXchange)

install(TARGETS nano
        RUNTIME DESTINATION bin
)



