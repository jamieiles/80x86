FROM s80x86-build-quartus:latest
MAINTAINER Jamie Iles <jamie@jamieiles.com>
ENV DEBIAN_FRONTEND=noninteractive
ARG MIRROR=https://sourcery.mentor.com/GNUToolchain/package14960/public/ia16-elf/
ARG TOOLCHAIN=sourceryg++-2016.11-64-ia16-elf-x86_64-linux-gnu.tar.bz2
ARG VERILATOR_VERSION=3.908
RUN apt-get update && apt-get install --no-install-recommends -y \
        clang \
        clang-c++ \
        cmake \
        cpp \
        git \
        nasm \
        libsdl2-2.0-0 \
        libsdl2-dev \
        libboost-serialization-dev \
        libboost-program-options-dev \
        libboost-python1.62.0 \
        libboost-python-dev \
        libusb-1.0-0-dev \
        python-dev \
        llvm \
        ninja-build \
        ccache \
        ruby \
        ruby-dev \
        python3 \
        python-distorm3 \
        python3-pip \
        python-pystache \
        python3-pystache \
        python-lxml \
        python-yaml \
        python3-yaml \
        wget \
        flex \
        bison \
        libfl-dev
RUN cd /tmp && \
        wget https://www.veripool.org/ftp/verilator-$VERILATOR_VERSION.tgz && \
        tar xf verilator-$VERILATOR_VERSION.tgz && \
        cd verilator-$VERILATOR_VERSION && \
        ./configure --prefix=/usr && \
        make -j4 && \
        make install && \
        cd /tmp && \
        rm verilator* -rf
RUN gem install --no-ri --pre prawn --version 2.1.0
RUN gem install --no-ri --pre \
        asciidoctor-pdf \
        coderay \
        pygments.rb \
        concurrent-ruby
RUN mkdir -p /build
WORKDIR /build
RUN apt-get install -y python3-setuptools
RUN pip3 install textX==1.6.1
RUN rm /usr/bin/gcov; ln -s /usr/bin/llvm-cov /usr/bin/gcov
RUN cd /tmp && \
        wget $MIRROR$TOOLCHAIN && \
        tar -C /usr --strip-components 1 -xf $TOOLCHAIN && \
        rm -rf $TOOLCHAIN
