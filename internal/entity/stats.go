package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type DamageReduction struct {
	Value      int
	BypassType rules.DamageType
}

type EnergyResistance struct {
	Value        int
	AffectedType rules.DamageType
}

type Stats struct {
	MaxHP int
	AC    int
	DR    []DamageReduction
	ER    []EnergyResistance
	Fort  int
}
