cmake_minimum_required(VERSION 3.16)
project(cpp_http_server_learning_journey CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

function(example target src)
  add_executable(${target} ${src})
  if(UNIX)
    target_link_libraries(${target} pthread)
  endif()
endfunction()

# Stage 1
example(copy_file_syscalls stages/stage1-system-io/copy_file_syscalls.cpp)
example(stage1_final stages/stage1-system-io/stage1_final.cpp)

