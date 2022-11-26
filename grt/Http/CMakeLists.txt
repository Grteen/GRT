set(http_srcs
    HttpMessage.cpp
    HttpRequest.cpp
    HttpResponse.cpp
    FormData.cpp

    HttpMessage.h
    HttpRequest.h
    HttpResponse.h
    FormData.h
)
add_library(grt_http_lib ${http_srcs})