package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type AttackContext struct {
	Attacker *Combatant
	Target   *Combatant
	Roll     int
	Hit      bool
}

type DamageContext struct {
	Attacker *Combatant
	Target   *Combatant
	Damage   []rules.DamageInstance
}

type CombatContext struct {
	Attacker *Combatant
	Target   *Combatant
	Attack   AttackResult
	Roller   dice.Roller
}

type HealContext struct {
	Source *Combatant
	Target *Combatant
	Amount int
}

type HitContext struct {
	Attacker *Combatant
	Target   *Combatant
}

type TurnContext struct {
	Target  *Combatant
	Enemies *int
}
