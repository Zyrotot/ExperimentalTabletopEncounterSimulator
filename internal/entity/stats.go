package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type Immunity struct {
	Type     rules.DamageType
	Category rules.DamageCategory
}

type Weakness struct {
	Value    int
	Type     rules.DamageType
	Category rules.DamageCategory
}

type DamageReduction struct {
	Value      int
	BypassType rules.DamageType
	Source     string
}

type EnergyResistance struct {
	Value        int
	AffectedType rules.DamageType
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
