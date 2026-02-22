APP := ETTES
BUILD_DIR := build
GEN := Ninja

configure:
	cmake -S . -B $(BUILD_DIR) -G $(GEN) -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug

configure-tests:
	cmake -S . -B $(BUILD_DIR) -G $(GEN) -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug

configure-release:
	cmake -S . -B $(BUILD_DIR) -G $(GEN) -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release

build: configure
	cmake --build $(BUILD_DIR) --target $(APP)

build-tests: configure-tests
	cmake --build $(BUILD_DIR) --target ETTES_Tests

build-release: configure-release
	cmake --build $(BUILD_DIR) --target $(APP)

run: build
	./$(BUILD_DIR)/$(APP)

BENCH_DIR := build-bench
BENCH_REPS := 3

configure-bench:
	cmake -S . -B $(BENCH_DIR) -G $(GEN) -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release

build-bench: configure-bench
	cmake --build $(BENCH_DIR) --target $(APP)

bench: build-bench
	@echo "=== Benchmark ($(BENCH_REPS) runs, Release build) ==="
	@for i in $$(seq 1 $(BENCH_REPS)); do \
		echo "--- Run $$i ---"; \
		./$(BENCH_DIR)/$(APP); \
	done

test: build-tests
	./$(BUILD_DIR)/tests/ETTES_Tests

clean:
	rm -rf $(BUILD_DIR)

ASAN_DIR := build-asan
TSAN_DIR := build-tsan

configure-asan:
	cmake -S . -B $(ASAN_DIR) -G $(GEN) -DBUILD_TESTS=ON -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug

build-asan: configure-asan
	cmake --build $(ASAN_DIR) --target ETTES_Tests

test-asan: build-asan
	./$(ASAN_DIR)/tests/ETTES_Tests

configure-tsan:
	cmake -S . -B $(TSAN_DIR) -G $(GEN) -DBUILD_TESTS=ON -DENABLE_TSAN=ON -DCMAKE_BUILD_TYPE=Debug

build-tsan: configure-tsan
	cmake --build $(TSAN_DIR) --target ETTES_Tests

test-tsan: build-tsan
	./$(TSAN_DIR)/tests/ETTES_Tests

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