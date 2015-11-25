add_subdirectory(iAntBase)
add_subdirectory(CPFA)
#add_subdirectory(DSA)

add_executable(iant_main main.cpp)

target_link_libraries(iant_main
        argos3core_simulator)