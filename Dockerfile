FROM golang
WORKDIR /app
COPY . .
RUN cd ./server/ && go build -ldflags="-w -s" -o app .

ENTRYPOINT [ "/app/server/app" ]
