package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type Immunity struct {
	Type     rules.DamageType
	Category rules.DamageCategory
}

type DamageReduction struct {
	Value      int
	BypassType rules.DamageType
}

type EnergyResistance struct {
	Value        int
	AffectedType rules.DamageType
}

type Stats struct {
	MaxHP      int
	AC         int
	Immunities []Immunity
	DR         []DamageReduction
	ER         []EnergyResistance
	Fort       int
}
