FROM ubuntu:xenial
MAINTAINER Jamie Iles <jamie@jamieiles.com>
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install --no-install-recommends -y \
        cmake \
        nasm \
        libsdl2-2.0-0 \
        libsdl2-dev \
        libsdl2-ttf-dev \
        libboost-python1.58.0 \
        g++ \
        libboost-python-dev \
        python-dev \
        verilator \
        lcov
RUN mkdir -p /build
WORKDIR /build
