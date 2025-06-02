FROM debian:stable-slim AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    libc6-dev \
    locales \
    git \
    sudo \
    fonts-firacode \
    xterm \
    fonts-dejavu \
    fonts-noto \
    && rm -rf /var/lib/apt/lists/*

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen

ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8
ENV TERM=xterm-256color

WORKDIR /app

RUN git clone https://github.com/LucasWerppFranco/Tusk-C-library.git && \
    cd Tusk-C-library && \
    sudo make install

COPY . .

RUN gcc -o main main.c -L/usr/local/lib -ltusk

FROM debian:stable-slim

RUN apt-get update && apt-get install -y \
    libc6 \
    xterm \
    fonts-firacode \
    fonts-dejavu \
    fonts-noto \
    locales \
    && rm -rf /var/lib/apt/lists/*

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen

COPY --from=build /app/main /app/main

COPY --from=build /app/map.txt /app/map.txt

COPY --from=build /app/data.h /app/data.h

COPY --from=build /usr/local/lib/libtusk.so /usr/local/lib/
RUN ldconfig

ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8
ENV TERM=xterm-256color

WORKDIR /app

CMD ["./main"]

