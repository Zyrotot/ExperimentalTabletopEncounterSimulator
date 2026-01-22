APP := simulator
CMD := .

run:
	go run $(CMD)

build:
	go build -o bin/$(APP) $(CMD)

test:
	go test ./...

lint:
	golangci-lint run

clean:
	rm -rf bin
