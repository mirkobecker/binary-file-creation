FROM mcr.microsoft.com/devcontainers/cpp:ubuntu

RUN apt-get update \
    && apt-get install -y gcc-mingw-w64-x86-64 valgrind clang-format \
    && rm -rf /var/lib/apt/lists/*
