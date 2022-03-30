FROM debian:buster
WORKDIR /home/usr
RUN apt-get update && apt-get install -y build-essential && \
    apt-get install -y gdb cmake vim git libgtk2.0-dev pkg-config \
    libavcodec-dev libavformat-dev libswscale-dev && \
    apt-get install -y python-dev python-numpy libtbb2 \
    libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev \
    && apt-get clean && rm -rf /var/lib/apt-lists/*

RUN apt-get install -y automake ca-certificates g++-8 \
    git libtool libleptonica-dev make pkg-config && \
    apt-get install -y --no-install-recommends asciidoc \
    docbook-xsl xsltproc && apt-get install -y libpango1.0-dev \
    && apt-get install -y libicu-dev libpango1.0-dev libcairo2-dev

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
          && make -j8 && make install

# build tesseract from source
RUN git clone https://github.com/tesseract-ocr/tesseract.git
RUN cd tesseract && git checkout tags/5.1.0 && cd ..

RUN cd tesseract && ./autogen.sh && ./configure \
    && make -j8 && make install && make training -j8 && \
    make training-install -j8 && ldconfig

# get tesseract trained data
RUN mkdir -p /usr/local/share/tessdata
RUN curl -o /usr/local/share/tessdata/eng.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/eng.traineddata

RUN curl -o /usr/local/share/tessdata/fra.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/fra.traineddata

RUN curl -o /usr/local/share/tessdata/osd.traineddata \
    https://raw.githubusercontent.com/tesseract-ocr/tessdata_best/master/osd.traineddata