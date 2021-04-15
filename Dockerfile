
FROM debian:bullseye-20210111

USER root

RUN apt-get update && apt-get install -y \
    build-essential git vim \
    python3 python3-pip
RUN python3 -m pip install cython

RUN g++ -v

COPY ./ /IMCtermite/

# install CLI tool
RUN cd /IMCtermite && ls -lh && make install && ls -lh /usr/local/bin/imctermite

# install Python module
RUN cd /IMCtermite && ls -lh && make cython-install

CMD ["sleep","infinity"]
