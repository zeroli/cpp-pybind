add_executable(example1
    example1.cc
)

target_include_directories(example1 PUBLIC ../src)
target_link_libraries(example1 PUBLIC python2.7)
