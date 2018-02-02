FROM ubuntu:xenial
MAINTAINER Jamie Iles <jamie@jamieiles.com>
ENV DEBIAN_FRONTEND=noninteractive

ARG MIRROR=http://download.altera.com/akdlm/software/acdsinst/
ARG QUARTUS=16.1/196/ib_installers/QuartusLiteSetup-16.1.0.196-linux.run
ARG QUARTUS_UPDATE=16.1.2/203/update/QuartusSetup-16.1.2.203-linux.run
ARG DEVICE_FILES="16.1/196/ib_installers/cyclonev-16.1.0.196.qdz 16.1/196/ib_installers/cyclone-16.1.0.196.qdz 16.1/196/ib_installers/max10-16.1.0.196.qdz"

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
        libcrypto++9v5

RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cd /tmp && wget -q $MIRROR$QUARTUS && \
        wget -q $MIRROR$QUARTUS_UPDATE && \
        chmod +x $(basename $QUARTUS_UPDATE) && \
        for DEV_FILE in $DEVICE_FILES; do wget -q $MIRROR$DEV_FILE; done && \
        chmod +x $(basename $QUARTUS) && \
        ./$(basename $QUARTUS) --mode unattended --installdir /opt/altera/ && \
        rm /tmp/* /opt/altera/uninstall -rf
# These files conflict with the Ubuntu versions and cause random segfaults.
RUN rm /opt/altera/quartus/linux64/libccl_curl_drl.so \
        /opt/altera/quartus/linux64/libcrypto.so.1.0.0 \
        /opt/altera/quartus/linux64/libcurl.so.4 \
        /opt/altera/quartus/linux64/libssl.so.1.0.0
