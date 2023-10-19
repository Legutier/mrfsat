FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/app

COPY ./src ./src
COPY CMakeLists.txt .
COPY entrypoint.sh /usr/src/app/

RUN mkdir build && cd build && cmake .. && make
ENTRYPOINT [ "build/mrfsat" ]
