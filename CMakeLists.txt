#cmake_minimum_required(VERSION 3.10)
	project(dndw)
	add_executable(dndw main.c)
	target_link_libraries(dndw PUBLIC "-lavformat")
