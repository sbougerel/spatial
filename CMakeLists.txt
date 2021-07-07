add_library(spatial INTERFACE)
target_include_directories(spatial SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp")
