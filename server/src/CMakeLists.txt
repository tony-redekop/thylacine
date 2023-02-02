set(SOURCE_FILES 
    main.cpp
	server.cpp
    )

# set variable in parent scope
set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)

# list(TRANSFORM SOURCE_FILES PREPEND ${CMAKE_SOURCE_DIR}/src/)

add_executable(demo ${SOURCE_FILES})

target_link_libraries(demo)
