package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type DamageReduction struct {
	Value      int
	BypassType rules.DamageType
}

type Stats struct {
	MaxHP int
	AC    int
	DR    []DamageReduction
	Fort  int
}
