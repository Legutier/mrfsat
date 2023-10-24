FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive


RUN apt-get update \
    && apt-get install -y g++ \
    && apt-get install -y make \
    && apt-get install -y build-essential cmake --no-install-recommends

WORKDIR /
RUN mkdir mrfsat/
ADD  . /mrfsat/
WORKDIR /mrfsat/build

RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make

WORKDIR /mrfsat

ENTRYPOINT [ "/mrfsat/build/mrfsat" ]
