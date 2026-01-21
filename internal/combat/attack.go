package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type DamageExpression struct {
	DamageTypes []rules.DamageType
	DamageRoll  dice.Expression
}

type Attack struct {
	Name               string
	DamageDice         []DamageExpression
	DamageContributors []DamageContributor
	AttackBonus        int
	CritRange          int
	CritBonus          int
}

type AttackResult struct {
	AttackRoll  int
	TotalAtk    int
	Hit         bool
	Crit        bool
	Damage      []rules.DamageInstance
	TotalDamage int
}
