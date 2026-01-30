APP := ETTES
BUILD_DIR := build
GEN := Ninja

configure:
	cmake -S . -B $(BUILD_DIR) -G $(GEN)

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(APP)

test: configure
	cmake --build $(BUILD_DIR)
	ctest --test-dir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
