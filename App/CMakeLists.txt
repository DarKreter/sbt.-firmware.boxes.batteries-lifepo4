add_executable(Firmware
        # Starting procedure
        Startup/main.cpp
        Startup/vector_table.cpp
        # Hardware abstraction
        Tasks/TaskManager.cpp
        # User code
        Tasks/BatterySystem.cpp
        Tasks/BatteryPack.cpp
)

target_include_directories(Firmware PRIVATE Tasks/)

target_link_libraries(Firmware PRIVATE STM32Cube-F1)
target_link_libraries(Firmware PRIVATE FreeRTOS-Kernel)
target_link_libraries(Firmware PRIVATE SBT-Hardware)

add_dependencies(Firmware STM32Cube-F1)
add_dependencies(Firmware FreeRTOS-Kernel)
add_dependencies(Firmware SBT-Hardware)