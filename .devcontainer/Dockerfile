FROM gcc:latest

ARG CMAKE_VERSION=3.23.2

RUN apt update && apt install -y libc6-dbg gdb valgrind

RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
    -q -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /usr/bin/cmake \
    && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
    && rm /tmp/cmake-install.sh 

ENV PATH="/usr/bin/cmake/bin:${PATH}"
