FROM s80x86-build-quartus:latest
LABEL maintainer="Jamie Iles <jamie@jamieiles.com>"
ENV DEBIAN_FRONTEND=noninteractive
ARG VERILATOR_VERSION=4.200
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
        libboost-python1.71.0 \
        libboost-python-dev \
        libboost-all-dev \
        libusb-1.0-0-dev \
        python-dev \
        llvm \
        ninja-build \
        ccache \
        python3 \
        python3-distorm3 \
        python3-pip \
        python3-pystache \
        python3-pystache \
        python-lxml \
        python-yaml \
        python3-yaml \
        software-properties-common \
        wget \
        flex \
        bison \
        libfl-dev \
        unzip \
        libssl-dev \
        libreadline-dev \
        zlib1g-dev \
        make \
        libyaml-dev \
        libffi-dev \
        libssl-dev
RUN add-apt-repository -y ppa:tkchia/build-ia16 && \
        apt-get update && \
        apt-get install -y gcc-ia16-elf
RUN cd /tmp && \
        wget https://github.com/verilator/verilator/archive/v$VERILATOR_VERSION.zip && \
        unzip v$VERILATOR_VERSION.zip && \
        cd verilator-$VERILATOR_VERSION && \
        autoconf && \
        ./configure --prefix=/usr && \
        make -j4 && \
        make install && \
        cd /tmp && \
        rm verilator* -rf
ENV RBENV_ROOT=/usr/local/rbenv
ENV PATH=${RBENV_ROOT}/bin:${PATH}
RUN echo 'export RBENV_ROOT="/usr/local/rbenv'
RUN echo 'export PATH="$RBENV_ROOT/bin:$PATH"' >> /root/.profile
RUN git clone https://github.com/sstephenson/rbenv.git ${RBENV_ROOT}
RUN git clone https://github.com/sstephenson/ruby-build.git ${RBENV_ROOT}/plugins/ruby-build
RUN rbenv init -
RUN rbenv install 3.3.10
RUN rbenv global 3.3.10
RUN rbenv exec gem update --system
RUN rbenv exec gem install \
        prawn \
        asciidoctor-pdf \
        coderay \
        pygments.rb \
        concurrent-ruby
RUN rbenv rehash
RUN ln -sf /usr/local/rbenv/versions/3.3.10/bin/* /usr/local/bin/
RUN mkdir -p /build
WORKDIR /build
RUN apt-get install -y python3-setuptools
RUN pip3 install textX==1.6.1
RUN rm /usr/bin/gcov; ln -s /usr/bin/llvm-cov /usr/bin/gcov
