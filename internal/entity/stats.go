package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type DamageReduction struct {
	Value       int
	BypassTypes []rules.DamageType
}

type Stats struct {
	MaxHP int
	AC    int
	DR    []DamageReduction
	Fort  int
}
