add_executable(nanogram main.cpp)
target_link_libraries(nanogram vsg::vsg vsgXchange::vsgXchange)
install(TARGETS nanogram RUNTIME DESTINATION bin)

add_executable(array2d array2d.cpp)
target_link_libraries(array2d vsg::vsg vsgXchange::vsgXchange)
install(TARGETS array2d RUNTIME DESTINATION bin)

add_executable(cppgrid "cppgrid.cpp")
install(TARGETS cppgrid RUNTIME DESTINATION bin)

add_executable(firstattempt "firstattempt.cpp")
target_link_libraries(firstattempt vsg::vsg vsgXchange::vsgXchange)
install(TARGETS firstattempt RUNTIME DESTINATION bin)

add_executable(secondattempt "secondattempt.cpp")
target_link_libraries(secondattempt vsg::vsg vsgXchange::vsgXchange)
install(TARGETS secondattempt RUNTIME DESTINATION bin)

