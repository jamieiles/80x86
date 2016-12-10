FROM ubuntu:xenial
MAINTAINER Jamie Iles <jamie@jamieiles.com>
RUN apt-get update && apt-get install -y \
        cmake \
        nasm \
        libsdl2-2.0-0 \
        libsdl2-dev \
        libsdl2-ttf-dev \
        libboost-python1.58.0 \
        g++ \
        libboost-python-dev \
        python-dev \
        git \
        verilator && rm -rf /var/cache/apt
RUN mkdir -p /build
WORKDIR /build
