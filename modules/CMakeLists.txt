cmake_minimum_required(VERSION 3.10)

if(LD_CORE_ENABLE_BOX2D)
    add_subdirectory(Box2d)
endif()

if(LD_CORE_ENABLE_LUAGAME)
    add_subdirectory(ldLuaGame)
endif()

if(LD_CORE_ENABLE_RAZER)
    add_subdirectory(ldRazer)
endif()

