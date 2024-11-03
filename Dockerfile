FROM debian:stable-slim
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y gcc make python3
COPY . /usr/src/nmark
WORKDIR /usr/src/nmark
RUN make clean && make && make install
ENTRYPOINT nmark /dev/stdin /dev/stdout
