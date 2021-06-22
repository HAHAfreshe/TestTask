FROM gcc:latest as build
WORKDIR /gtest_build
RUN apt-get update && \
    apt-get install -y gcc libc6-dev
ADD ./src /app/src
WORKDIR /app/build
RUN gcc ../src/TestTask.c -o TestTask

FROM ubuntu:latest
RUN groupadd -r testgrp && useradd -r -g testgrp testusr
USER testusr
WORKDIR /app
COPY --from=build /app/build/TestTask .
ENTRYPOINT ["./TestTask"]
