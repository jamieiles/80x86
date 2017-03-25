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
        lcov \
        ccache \
        ruby \
        ruby-dev \
        python3 \
        python3-pip \
        python3-pystache \
        python-lxml \
        wget \
        flex \
        bison
RUN cd /tmp && \
        wget https://www.veripool.org/ftp/verilator-3.900.tgz && \
        tar xf verilator-3.900.tgz && \
        cd verilator-3.900 && \
        ./configure --prefix=/usr && \
        make -j4 && \
        make install && \
        cd /tmp && \
        rm verilator* -rf
RUN gem install --no-ri --pre prawn --version 2.1.0
RUN gem install --no-ri --pre \
        asciidoctor-pdf \
        coderay \
        pygments.rb
RUN mkdir -p /build
WORKDIR /build
RUN apt-get install -y python3-setuptools
RUN pip3 install textX
