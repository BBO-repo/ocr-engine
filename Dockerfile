FROM debian:bullseye
WORKDIR /home/usr

# update distribution and install dependencies
RUN apt update && apt install -y --no-install-recommends \
    pkg-config build-essential libssl-dev wget gdb git \
    libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev \
    python3-dev python3-numpy libtbb-dev curl \
    libjpeg-dev libpng-dev libtiff-dev libicu-dev \
    libpango1.0-dev libcairo2-dev libtool libleptonica-dev \
    asciidoc docbook-xsl xsltproc ca-certificates \
    automake libtool \
    && apt clean && rm -rf /var/lib/apt-lists/*

# install latest cmake from source
RUN wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.23.1/cmake-3.23.1.tar.gz && \
    tar -zxvf cmake-3.23.1.tar.gz && \
    cd cmake-3.23.1/ && \
    ./bootstrap && make -j12 && make install

# build opencv from source
RUN git clone https://github.com/opencv/opencv.git && \
    cd opencv && git checkout tags/4.5.5 && cd .. && \
    git clone https://github.com/opencv/opencv_contrib.git && \
    cd opencv_contrib && git checkout tags/4.5.5 && cd .. && \
    cd opencv && mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr/local \
          -DOPENCV_GENERATE_PKGCONFIG=ON \ 
          -DWITH_QT=ON \
          -DOPENCV_EXTRA_MODULES_PATH=/home/usr/opencv_contrib/modules .. \
          && make -j12 && make install

# build tesseract from source
RUN git clone https://github.com/tesseract-ocr/tesseract.git && \
    cd tesseract && git checkout tags/5.1.0 && cd .. && \
    cd tesseract && ./autogen.sh && ./configure \
    && make -j12 && make install && make training -j12 && \
    make training-install -j12 && ldconfig && \
    # get tesseract trained data
    mkdir -p /usr/local/share/tessdata && \
    curl -o /usr/local/share/tessdata/eng.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/eng.traineddata && \
    curl -o /usr/local/share/tessdata/fra.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/fra.traineddata && \
    curl -o /usr/local/share/tessdata/osd.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/osd.traineddata

# remove no more used download 
RUN rm -rf cmake-3.23* opencv opencv_contrib tesseract 