FROM ubuntu:22.04

LABEL authors="Alexander"

WORKDIR /app

COPY . /app

RUN make

ENTRYPOINT ["top", "-b"]