# Build Stage
FROM --platform=linux/amd64 ubuntu:20.04 as builder
# SHELL ["/bin/bash", "-c"]
RUN apt-get update
# RUN DEBIAN_FRONTEND=noninteractive apt-get install  -y vim less man wget tar git gzip unzip make cmake software-properties-common
RUN apt-get install -y clang gdb lldb cmake ninja-build

ADD . /btstack
WORKDIR /btstack/test/fuzz/build
RUN cmake ..
RUN make

FROM --platform=linux/amd64 ubuntu:20.04 

COPY --from=builder /btstack/test/fuzz/build/fuzz_ad_parser .
