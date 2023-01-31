FROM ubuntu:22.10

ENV CLANG_FORMAT_VERSION=15
ENV CLANG_TIDY_VERSION=15

RUN apt update && \
    apt install -y \
        clang-format-${CLANG_FORMAT_VERSION} \
        clang-tidy-${CLANG_TIDY_VERSION} 
