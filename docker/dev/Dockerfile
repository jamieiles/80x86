FROM s80x86-build:latest
MAINTAINER Jamie Iles <jamie@jamieiles.com>
RUN apt-get install -y runit \
        gtkwave \
        gdb \
        sudo \
        ninja-build
ADD s80x86-dev /usr/bin/s80x86-dev
ENTRYPOINT /usr/bin/s80x86-dev
