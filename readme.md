# Experimental TableTop Encounter Simulator

A small simulator for testing combat balance (originally for TormentaRPG). It runs encounter simulations between characters and monsters using simple d20-style mechanics. The code is written in Go and organized under the `internal/` package.

## Quick Start

Requirements:

- Go (1.18+ recommended)

Run the simulator:

```bash
make run
```

Build a binary:

```bash
make build
```

Run tests:

```bash
make test
```

## Project layout

Top-level files:

- `main.go` — program entry point and example runner
- `go.mod` — Go module file
- `makefile` — convenience build/run targets

Relevant packages under `internal/`:

- `arena` — arena and encounter orchestration
- `combat` — attack resolution, effects, damage sources, and the combat resolver
- `dice` — dice rolling utilities
- `engine` — simulation engine core
- `entity` — character and entity definitions and stats
- `factories` — helper functions to create characters and monsters
- `logging` — centralized logger
- `rules` — domain-specific rules (e.g., damage types)
- `simulator` — higher-level simulation orchestration

## Purpose and scope

This project is intended as a lightweight tool to prototype and compare character builds and monster designs. It's not a full rules implementation — it focuses on deterministic simulation of basic combat flows and dice-based randomness.

## TODO (planned features)

- Support for large creatures
- Add movement to the combat flow

## License

See the `LICENSE` file in the repository.

---
