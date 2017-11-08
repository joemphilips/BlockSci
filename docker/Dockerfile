from ubuntu:16.04
MAINTAINER Joe Miyamoto <joemphilips@gmail.com>
SHELL ["/bin/bash", "-c"]

RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app


RUN set -e \
    && apt-get update

RUN set -e \
  && apt-get install --yes \
  build-essential cmake libssl-dev libboost-all-dev libsqlite3-dev autogen \
  autoconf libleveldb-dev libcurl4-openssl-dev libjsoncpp-dev libjsonrpccpp-dev libjsonrpccpp-tools \
  python3-dev python3-pip git

RUN apt-get install -y software-properties-common # for add-apt-repository

RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y \
   && apt-get update \
   && apt install -y gcc-6 g++-6 \
   && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-6

RUN set -e \
    && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN set -e \
  && git clone --single-branch -b master https://github.com/bitcoin-core/secp256k1 \
  && cd secp256k1 \
  && ./autogen.sh \
  && ./configure --enable-module-recovery \
  && make \
  && make install


RUN set -e \
  && git clone --single-branch -b master https://github.com/citp/BlockSci.git \
  && cd BlockSci \
  && git submodule update --init --recursive --checkout


RUN set -e \
  && cd BlockSci/libs/bitcoin-api-cpp \
  && mkdir release \
  && cd release \
  && cmake --DCMAKE_BUILD_TYPE=Release .. \
  && make \
  && make install \
  && cd ../../.. \
  && mkdir release \
  && cd release \
  && cmake --DCMAKE_BUILD_TYPE=Release .. \
  && make \
  && make install

RUN set -e \
  && pip3 install --upgrade pip \
  && pip3 install --upgrade multiprocess psutil jupyter pycrypto matplotlib pandas dateparser



# COPY package.json /usr/src/app/package.json

# RUN npm i --production --no-progress && npm cache verify
