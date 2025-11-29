cmake_minimum_required(VERSION 3.15)
project(MediaUploadService)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Find required packages
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# Proto file
set(PROTO_FILES media_service.proto)

# Generate gRPC and Protobuf files
get_target_property(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_RELEASE)
get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_RELEASE)

# Fallback for debug configuration
if(NOT PROTOBUF_PROTOC_EXECUTABLE)
    get_target_property(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_DEBUG)
endif()
if(NOT GRPC_CPP_PLUGIN_EXECUTABLE)
    get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_DEBUG)
endif()

set(PROTO_SRCS "${CMAKE_CURRENT_BINARY_DIR}/media_service.pb.cc")
set(PROTO_HDRS "${CMAKE_CURRENT_BINARY_DIR}/media_service.pb.h")
set(GRPC_SRCS "${CMAKE_CURRENT_BINARY_DIR}/media_service.grpc.pb.cc")
set(GRPC_HDRS "${CMAKE_CURRENT_BINARY_DIR}/media_service.grpc.pb.h")

add_custom_command(
    OUTPUT ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS}
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
    ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
         --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         -I${CMAKE_CURRENT_SOURCE_DIR}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
         ${CMAKE_CURRENT_SOURCE_DIR}/media_service.proto
    DEPENDS ${PROTO_FILES}
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Producer executable
add_executable(producer_client
    src/producerMain.cpp
    src/producerClient.cpp
    src/producerThread_enhanced.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_link_libraries(producer_client
    gRPC::grpc++
    protobuf::libprotobuf
    Threads::Threads
)

# Windows-specific libraries for producer
if(WIN32)
    target_link_libraries(producer_client ws2_32 wsock32)
endif()

# Consumer/Server executable
add_executable(consumer_server
    src/serverMain.cpp
    src/consumerServer.cpp
    src/webServer.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_link_libraries(consumer_server
    gRPC::grpc++
    protobuf::libprotobuf
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

# Windows-specific libraries for server
if(WIN32)
    target_link_libraries(consumer_server ws2_32 wsock32)
endif()

# Copy web directory to build
add_custom_command(TARGET consumer_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/web $<TARGET_FILE_DIR:consumer_server>/web
)

# Installation
install(TARGETS producer_client consumer_server
    RUNTIME DESTINATION bin
)

install(DIRECTORY web DESTINATION share/media-upload-service)
