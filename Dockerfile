FROM fedora:42
RUN dnf makecache --refresh
RUN dnf install -y \
    cmake \
    ninja \
    arm-none-eabi-gcc \
    arm-none-eabi-g++ \
    arm-none-eabi-newlib
WORKDIR /repo
SHELL [ "bash", "-c" ]
ENTRYPOINT umask 000; \
    cmake -B build/ --preset Release && \
    cmake --build build/
