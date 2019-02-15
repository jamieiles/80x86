FROM ubuntu:bionic
MAINTAINER Jamie Iles <jamie@jamieiles.com>
ENV DEBIAN_FRONTEND=noninteractive

ARG MIRROR=http://download.altera.com/akdlm/software/acdsinst/
ARG QUARTUS=18.1std/625/ib_installers/QuartusLiteSetup-18.1.0.625-linux.run
ARG QUARTUS_UPDATE=""
ARG DEVICE_FILES="18.1std/625/ib_installers/max10-18.1.0.625.qdz 18.1std/625/ib_installers/cyclone-18.1.0.625.qdz 18.1std/625/ib_installers/cyclonev-18.1.0.625.qdz"

RUN dpkg --add-architecture i386
RUN apt-get update && apt-get install --no-install-recommends -y \
        ca-certificates \
        openjdk-8-jdk \
        libfontconfig1 \
        libglib2.0-0 \
        libsm6 \
        libxrender1 \
        locales \
        make \
        xauth \
        xvfb \
        pkg-config \
        wget \
        libxft2:i386 \
        libxext6:i386 \
        libncurses5:i386 \
        libc6:i386 \
        libstdc++6:i386 \
        unixodbc-dev \
        lib32ncurses5-dev \
        libzmq3-dev \
        libssl-dev \
        libcrypto++-dev \
        libcrypto++6

# Quartus 18.1 needs libpng12 which is no longer present in 18.04
RUN cd /tmp; \
        wget http://security.ubuntu.com/ubuntu/pool/main/libp/libpng/libpng12-0_1.2.54-1ubuntu1.1_amd64.deb && \
        dpkg -i libpng12-0_1.2.54-1ubuntu1.1_amd64.deb && \
        rm libpng12-0_1.2.54-1ubuntu1.1_amd64.deb

RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cd /tmp && wget -q $MIRROR$QUARTUS && \
        for DEV_FILE in $DEVICE_FILES; do wget -q $MIRROR$DEV_FILE; done && \
        chmod +x $(basename $QUARTUS) && \
        ./$(basename $QUARTUS) --accept_eula 1 --mode unattended --installdir /opt/altera/ && \
        rm /tmp/* /opt/altera/uninstall -rf
