package combat

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type AttackContext struct {
	Attacker *Combatant
	Target   *Combatant
	Roll     int
	Hit      bool
}

type DamageContext struct {
	Attacker *Combatant
	Target   *Combatant
	Damage   map[rules.DamageType]int
}

type HitContext struct {
	Attacker *Combatant
	Target   *Combatant
}

type TurnContext struct {
	Target  *Combatant
	Enemies *int
}
