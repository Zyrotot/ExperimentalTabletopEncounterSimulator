APP := ETTES
BUILD_DIR := build
GEN := Ninja

configure:
	cmake -S . -B $(BUILD_DIR) -G $(GEN) -DBUILD_TESTS=OFF

configure-tests:
	cmake -S . -B $(BUILD_DIR) -G $(GEN) -DBUILD_TESTS=ON

build: configure
	cmake --build $(BUILD_DIR) --target $(APP)

build-tests: configure-tests
	cmake --build $(BUILD_DIR) --target ETTES_Tests

run: build
	./$(BUILD_DIR)/$(APP)

test: build-tests
	./$(BUILD_DIR)/tests/ETTES_Tests

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