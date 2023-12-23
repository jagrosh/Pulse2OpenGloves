# CMakeLists.txt
# cmake version 3.28.1
# (c) jagrosh

cmake_minimum_required(VERSION 3.12)

project(Pulse2OpenGloves)
  file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
  add_executable(Pulse2OpenGloves "src/main.cpp")
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCE_FILES})

  # add hidapi dependency
  find_library(HIDAPI hidapi HINTS "dependencies" "dependencies/hidapi-win" "dependencies/hidapi-win/x64" REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${HIDAPI})
  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "dependencies/hidapi-win/include")
  add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x64/hidapi.dll"
        $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)