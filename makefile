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

coverage:
	rm -rf build-coverage coverage.info coverage-report
	cmake -S . -B build-coverage -G $(GEN) -DENABLE_COVERAGE=ON
	cmake --build build-coverage
	ctest --test-dir build-coverage
	lcov --capture --directory build-coverage --output-file coverage.info
	lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
	genhtml coverage.info --output-directory coverage-report