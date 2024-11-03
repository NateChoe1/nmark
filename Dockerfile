FROM debian:stable-slim
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y gcc make
COPY . /usr/src/nmark
WORKDIR /usr/src/nmark
RUN make && make install
ENTRYPOINT nmark /dev/stdin /dev/stdout
