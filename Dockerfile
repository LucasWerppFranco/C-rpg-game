# Etapa de build
FROM debian:stable-slim AS build

# Atualizando e instalando dependências necessárias
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    libc6-dev \
    locales \
    git \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Configuração de locais
RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

# Diretório de trabalho
WORKDIR /app

# Clonar o repositório Tusk e instalar a biblioteca
RUN git clone https://github.com/LucasWerppFranco/Tusk-C-library.git && \
    cd Tusk-C-library && \
    sudo make install

# Copiar os arquivos do projeto principal para o container
COPY . .

# Compilar o projeto principal, linkando a biblioteca Tusk
RUN gcc -o main main.c -L/usr/local/lib -ltusk

# Etapa de execução
FROM debian:stable-slim

# Instalar as dependências de execução
RUN apt-get update && apt-get install -y \
    libc6 \
    && rm -rf /var/lib/apt/lists/*

# Copiar o binário compilado do estágio anterior
COPY --from=build /app/main /app/main

# Adicionar a biblioteca ao cache do ldconfig
COPY --from=build /usr/local/lib/libtusk.so /usr/local/lib/
RUN ldconfig

# Definir a variável de ambiente para o caminho das bibliotecas
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Definir diretório de trabalho
WORKDIR /app

# Executar o programa
CMD ["./main"]

