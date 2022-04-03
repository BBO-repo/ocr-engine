FROM debian:buster
WORKDIR /home/usr

# update distribution
RUN apt update

# install build essential
RUN apt install -y build-essential libssl-dev wget

# install latest cmake from source
RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.0/cmake-3.23.0.tar.gz && \
    tar -zxvf cmake-3.23.0.tar.gz && \
    cd cmake-3.23.0/ && \
    ./bootstrap && make -j12 && make install

# install dependencies for tesseract
RUN apt install -y gdb vim git libgtk2.0-dev pkg-config \
    libavcodec-dev libavformat-dev libswscale-dev && \
    apt-get install -y python-dev python-numpy libtbb2 \
    libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev \
    && apt clean && rm -rf /var/lib/apt-lists/*

RUN apt install -y automake ca-certificates g++-8 \
    libtool libleptonica-dev make pkg-config && \
    apt-get install -y --no-install-recommends asciidoc \
    docbook-xsl xsltproc && apt install -y libpango1.0-dev \
    && apt install -y libicu-dev libpango1.0-dev libcairo2-dev

RUN apt install  -y libcanberra-gtk-module libcanberra-gtk3-module

# build opencv from source
RUN git clone https://github.com/opencv/opencv.git
RUN cd opencv && git checkout tags/4.5.5 && cd ..
RUN git clone https://github.com/opencv/opencv_contrib.git
RUN cd opencv_contrib && git checkout tags/4.5.5 && cd ..

RUN cd opencv && mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          -DOPENCV_GENERATE_PKGCONFIG=ON \ 
          -DWITH_QT=ON \
          -DOPENCV_EXTRA_MODULES_PATH=/home/usr/opencv_contrib/modules .. \
          && make -j12 && make install

# build tesseract from source
RUN git clone https://github.com/tesseract-ocr/tesseract.git
RUN cd tesseract && git checkout tags/5.1.0 && cd ..

RUN cd tesseract && ./autogen.sh && ./configure \
    && make -j12 && make install && make training -j12 && \
    make training-install -j12 && ldconfig

# get tesseract trained data
RUN apt install -y curl

RUN mkdir -p /usr/local/share/tessdata
RUN curl -o /usr/local/share/tessdata/eng.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/eng.traineddata

RUN curl -o /usr/local/share/tessdata/fra.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/fra.traineddata

RUN curl -o /usr/local/share/tessdata/osd.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/osd.traineddata