FROM python:2.7-stretch
RUN apt-get update && apt-get install -y gcc-arm-none-eabi screen clang-tidy ruby-full
WORKDIR /usr/src/app
RUN curl -sSL https://cmake.org/files/v3.17/cmake-3.17.0-rc1-Linux-x86_64.tar.gz | tar -xzC /opt
RUN pip install jumper mbed-cli click
ENV  PATH=$PATH:/opt/cmake-3.17.0-rc1-Linux-x86_64/bin
