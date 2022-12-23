FROM ubuntu:22.04
RUN apt-get update && apt-get upgrade -y
RUN apt-get update && apt-get install -y build-essential make
COPY . /HermesFS
COPY apps /apps
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/out/lib
RUN cd /HermesFS && make OUT=/out
RUN cd /apps && make OUT=/out
EXPOSE 5011/udp
CMD cd /out && ./hermes
