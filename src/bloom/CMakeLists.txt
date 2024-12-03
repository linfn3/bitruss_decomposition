aux_source_directory(. srcs)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(bloom . ${srcs}
        ../../include/utils/my_vector.h)