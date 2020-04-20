FROM alpine
RUN apk add gcc-arm-none-eabi-stage1 screen clang ruby cmake
COPY . .
