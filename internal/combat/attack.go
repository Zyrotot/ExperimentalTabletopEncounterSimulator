package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type DamageExpression struct {
	DamageTypes []rules.DamageType
	DamageRoll  dice.Term
}

type Attack struct {
	Name            string
	DamageDice      []DamageExpression
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
	Damage      []rules.DamageInstance
	TotalDamage int
}
