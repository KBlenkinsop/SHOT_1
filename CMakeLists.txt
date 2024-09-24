message(STATUS "Configuring: ${TARGET_NAME_PIGEON} projects")

sub_dir_list(${CMAKE_CURRENT_SOURCE_DIR} false FOLDER_NAMES)

foreach(FOLDER_NAME ${FOLDER_NAMES})
	build_project(${FOLDER_NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${TARGET_NAME_PIGEON} pigeon)
endforeach(FOLDER_NAME)
