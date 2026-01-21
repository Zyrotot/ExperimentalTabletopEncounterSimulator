package engine

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"

var log *logging.Logger

func SetLogger(l *logging.Logger) {
	log = l
}
