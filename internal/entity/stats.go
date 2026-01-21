package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type Weakness struct {
	Value  int
	Filter DamageFilter
}
type Immunity struct {
	Filter DamageFilter
}

type EnergyResistance struct {
	Value  int
	Filter DamageFilter
}

type DamageReduction struct {
	Value      int
	BypassType rules.DamageType
	Source     string
}

type Stats struct {
	MaxHP      int
	AC         int
	Immunities []Immunity
	Weaknesses []Weakness
	DR         []DamageReduction
	ER         []EnergyResistance
	Fort       int
}
