
set(SOURCES
    main.cpp
)

add_executable(nanogram ${SOURCES})

target_link_libraries(nanogram vsg::vsg vsgXchange::vsgXchange)

install(TARGETS nanogram
        RUNTIME DESTINATION bin
)



