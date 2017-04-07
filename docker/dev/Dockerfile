FROM s80x86-build-quartus:latest
MAINTAINER Jamie Iles <jamie@jamieiles.com>
RUN apt-get install -y runit \
        gtkwave \
        gdb \
        sudo \
        ninja-build
ADD s80x86-dev /usr/bin/s80x86-dev
ENV CTEST_OUTPUT_ON_FAILURE=1
ENV GTEST_COLOR=1
ENTRYPOINT ["/usr/bin/s80x86-dev"]
