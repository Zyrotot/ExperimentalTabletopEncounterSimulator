package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
)

type DamageType string

const (
	Slash    DamageType = "slash"
	Pierce   DamageType = "pierce"
	Blunt    DamageType = "blunt"
	Magic    DamageType = "magic"
	Fire     DamageType = "fire"
	Cold     DamageType = "cold"
	Acid     DamageType = "acid"
	Electric DamageType = "electric"
	Silver   DamageType = "silver"
	Evil     DamageType = "evil"
	Good     DamageType = "good"
	Chaotic  DamageType = "chaotic"
	Lawful   DamageType = "lawful"
)

type DamageExpression struct {
	Components map[DamageType]dice.Term
}

type Attack struct {
	Name            string
	DamageDice      DamageExpression
	DamageModifiers []DamageModifier
	AttackBonus     int
	CritRange       int
	CritBonus       int
}

type AttackResult struct {
	AttackRoll  int
	TotalAtk    int
	Hit         bool
	Crit        bool
	Damage      map[DamageType]int
	TotalDamage int
}
