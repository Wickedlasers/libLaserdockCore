if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
else()
    set(LINUX FALSE)
endif()
