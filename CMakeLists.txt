project(dndw)
add_executable(dndw main.c)

target_link_libraries(dndw PUBLIC "-lavformat -larchive -lavutil -ljansson -latomic")
