# Stage 1: Build
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential cmake libgrpc++-dev libprotobuf-dev \
    protobuf-compiler-grpc libssl-dev pkg-config git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# --- AUTOMATED FIXES ---
RUN sed -i '1i#include <iomanip>' src/producerClient.cpp
RUN sed -i 's/find_package(Protobuf CONFIG REQUIRED)/find_package(Protobuf REQUIRED)/' CMakeLists.txt
RUN sed -i 's/find_package(gRPC CONFIG REQUIRED)/find_package(PkgConfig REQUIRED)\npkg_check_modules(gRPC REQUIRED IMPORTED_TARGET grpc++)\nadd_library(gRPC::grpc++ ALIAS PkgConfig::gRPC)/' CMakeLists.txt
RUN sed -i 's/get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_RELEASE)/find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)/' CMakeLists.txt \
    && sed -i 's/get_target_property(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_RELEASE)/find_program(PROTOBUF_PROTOC_EXECUTABLE protoc)/' CMakeLists.txt
RUN sed -i 's/get_target_property(GRPC_CPP_PLUGIN_EXECUTABLE gRPC::grpc_cpp_plugin IMPORTED_LOCATION_DEBUG)/# Debug fallback removed/' CMakeLists.txt \
    && sed -i 's/get_target_property(PROTOBUF_PROTOC_EXECUTABLE protobuf::protoc IMPORTED_LOCATION_DEBUG)/# Debug fallback removed/' CMakeLists.txt
# -----------------------

RUN rm -rf build && mkdir build && cd build \
    && cmake .. \
    && make -j$(nproc)

# Stage 2: Runtime
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libgrpc++1 libprotobuf23 libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/consumer_server /app/
COPY --from=builder /app/build/producer_client /app/
COPY --from=builder /app/build/web /app/web

RUN mkdir -p /app/uploaded_videos /app/video_files


RUN echo '#!/bin/bash\n\
\n\
# Default to 1 if not found\n\
NUM_PRODUCERS=1\n\
\n\
# Parse arguments to find -p value\n\
args=("$@")\n\
for ((i=0; i<$#; i++)); do\n\
    if [[ "${args[i]}" == "-p" ]]; then\n\
        NUM_PRODUCERS="${args[i+1]}"\n\
        break\n\
    fi\n\
done\n\
\n\
echo "--------------------------------------------------------"\n\
echo "⚡ AUTO-SETUP: Preparing folders for $NUM_PRODUCERS producers..."\n\
echo "--------------------------------------------------------"\n\
\n\
for i in $(seq 1 $NUM_PRODUCERS); do\n\
    dir="/app/video_files/producer_$i"\n\
    mkdir -p "$dir"\n\
    # Create a dummy video file if folder is empty\n\
    if [ -z "$(ls -A $dir)" ]; then\n\
       echo "  > [Created] $dir/sample_video.mp4"\n\
       # Create a 1MB dummy file named as an MP4\n\
       dd if=/dev/urandom of="$dir/sample_video.mp4" bs=1M count=1 status=none\n\
    else\n\
       echo "  > [Exists ] $dir"\n\
    fi\n\
done\n\
\n\
echo "--------------------------------------------------------"\n\
echo "🚀 Starting Producer Client..."\n\
echo "--------------------------------------------------------"\n\
\n\
# Execute the original command passed to docker run\n\
exec "$@"\n\
' > /app/producer_entrypoint.sh && chmod +x /app/producer_entrypoint.sh
# -----------------------------------------------------------------------------

EXPOSE 50051 8080

CMD ["./consumer_server"]