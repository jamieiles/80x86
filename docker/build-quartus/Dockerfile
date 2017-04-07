FROM ubuntu:xenial
MAINTAINER Jamie Iles <jamie@jamieiles.com>
ENV DEBIAN_FRONTEND=noninteractive

ARG MIRROR=http://download.altera.com/akdlm/software/acdsinst/14.1/186/ib_installers/
ARG QUARTUS=QuartusSetupWeb-14.1.0.186-linux.run
ARG DEVICE_FILE=cyclonev-14.1.0.186.qdz

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
        libzmq3-dev

RUN echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen && locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

RUN cd /tmp && wget -q $MIRROR$QUARTUS && \
        wget -q $MIRROR$DEVICE_FILE && \
        chmod +x $QUARTUS && \
        ./$QUARTUS --mode unattended --installdir /opt/altera/ && \
        rm /tmp/* /opt/altera/uninstall -rf
RUN echo 'PATH=/opt/altera/quartus/bin:$PATH' >> /etc/environment
# These files conflict with the Ubuntu versions and cause random segfaults.
RUN rm /opt/altera/quartus/linux64/libccl_curl_drl.so \
        /opt/altera/quartus/linux64/libcrypto.so.1.0.0 \
        /opt/altera/quartus/linux64/libcurl.so.4 \
        /opt/altera/quartus/linux64/libssl.so.1.0.0
