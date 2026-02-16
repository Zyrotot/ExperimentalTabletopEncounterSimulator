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

COVERAGE_DIR  := build-coverage
COVERAGE_INFO := coverage.info
COVERAGE_RPT  := coverage-report
LCOV_OPTS     := --rc lcov_branch_coverage=1

coverage:
	rm -rf $(COVERAGE_DIR) $(COVERAGE_INFO) $(COVERAGE_RPT)
	cmake -S . -B $(COVERAGE_DIR) -G $(GEN) -DENABLE_COVERAGE=ON
	cmake --build $(COVERAGE_DIR)
	@# Capture baseline (zero counters) so untested files show 0%
	lcov $(LCOV_OPTS) --capture --initial --directory $(COVERAGE_DIR) \
	     --output-file coverage-base.info
	@# Run tests
	ctest --test-dir $(COVERAGE_DIR) --output-on-failure
	@# Capture post-test counters
	lcov $(LCOV_OPTS) --capture --directory $(COVERAGE_DIR) \
	     --output-file coverage-test.info
	@# Merge baseline + test counters
	lcov $(LCOV_OPTS) --add-tracefile coverage-base.info \
	     --add-tracefile coverage-test.info \
	     --output-file $(COVERAGE_INFO)
	@# Strip system headers, vendored deps, tests, and build artifacts
	lcov $(LCOV_OPTS) --remove $(COVERAGE_INFO) \
	     '/usr/*' \
	     '*/external/*' \
	     '*/tests/*' \
	     '*/build-coverage/*' \
	     --output-file $(COVERAGE_INFO)
	@# Generate HTML report with branch coverage
	genhtml $(LCOV_OPTS) $(COVERAGE_INFO) --output-directory $(COVERAGE_RPT)
	@rm -f coverage-base.info coverage-test.info
	@echo "Coverage report: $(COVERAGE_RPT)/index.html"