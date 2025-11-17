FROM gcc:14 AS build

RUN apt-get update && \
    apt-get install -y cmake nlohmann-json3-dev python3 python3-pandas && \
    apt-get clean

WORKDIR /app

COPY . .

RUN cmake -B build -S . && \
    cmake --build build --config Release

FROM debian:stable-slim

RUN apt-get update && \
    apt-get install -y python3 python3-pandas nlohmann-json3-dev && \
    apt-get clean

WORKDIR /app/build

COPY --from=build /app /app

RUN chmod +x /app/build/grasp

CMD ["./grasp"]
