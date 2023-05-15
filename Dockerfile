FROM debian:latest AS build
WORKDIR /cadcam

COPY . ./src

ARG DEBIAN_FRONTEND=noninteractive
ARG DISPLAY=:0

RUN apt-get update && apt-get install -y --force-yes \
    libocct-data-exchange-7.5 \
    libocct-data-exchange-dev \
    libocct-foundation-7.5 \
    libocct-foundation-dev \
    libocct-modeling-algorithms-7.5 \
    libocct-modeling-algorithms-dev \
    libocct-modeling-data-7.5 \
    libocct-modeling-data-dev \
    libocct-ocaf-7.5 \
    libocct-ocaf-dev \
    libocct-visualization-7.5 \
    libocct-visualization-dev \
    libglfw3-dev libglfw3 \
    build-essential cmake \
    xorg mesa-utils libgl1-mesa-glx \
    dbus

RUN cmake -S src -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$((1 + $(nproc)))

ENTRYPOINT /cadcam/build/cadappcam
