cmake_minimum_required(VERSION 3.15)
project(MediaUploadProducer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Proto file
set(PROTO_FILES proto/media_service.proto)

# Generate gRPC code
add_custom_command(
    OUTPUT media_service.pb.cc media_service.pb.h 
           media_service.grpc.pb.cc media_service.grpc.pb.h
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
    ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
         --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         -I${CMAKE_CURRENT_SOURCE_DIR}/proto
         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
         ${CMAKE_CURRENT_SOURCE_DIR}/proto/media_service.proto
    DEPENDS ${PROTO_FILES}
    COMMENT "Generating gRPC code from proto files"
)

# Proto library
add_library(media_proto
    ${CMAKE_CURRENT_BINARY_DIR}/media_service.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/media_service.grpc.pb.cc
)

target_link_libraries(media_proto
    gRPC::grpc++
    protobuf::libprotobuf
)

target_include_directories(media_proto PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Producer executable
add_executable(producer
    src/main.cpp
    src/ProducerClient.cpp
    src/ProducerThread.cpp
)

target_link_libraries(producer
    media_proto
    gRPC::grpc++
    protobuf::libprotobuf
    Threads::Threads
)

# Install
install(TARGETS producer DESTINATION bin)