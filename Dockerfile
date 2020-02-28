FROM python:2.7-stretch
ENTRYPOINT /bin/bash
ENV DOCKER_JUMPER_EXAMPLES=1
RUN apt-get update && apt-get install -y gcc-arm-none-eabi screen
RUN mkdir ~/temp && cd ~/temp
RUN wget https://github.com/Kitware/CMake/releases/download/v3.17.0-rc1/cmake-3.17.0-rc1.tar.gz
RUN tar -xzvf cmake-3.17.0-rc1.tar.gz
RUN cd cmake-3.17.0-rc1/ && ./bootstrap && make && make install
RUN pip install jumper
RUN pip install mbed-cli 
RUN pip install click
RUN mbed config -G ARM_PATH /usr/bin 

