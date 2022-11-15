set(base_srcs
    Mutexoperator.h
    Noncopyable.h
    Timeoperator.h
    Timeoperator.cpp
)
add_library(grt_base_lib ${base_srcs})

target_link_libraries(grt_base_lib grt_net_lib)