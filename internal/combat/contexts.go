package combat

type AttackContext struct {
	Attacker *Combatant
	Target   *Combatant
	Roll     int
	Hit      bool
}

type DamageContext struct {
	Attacker *Combatant
	Target   *Combatant
	Damage   *int
}

type HitContext struct {
	Attacker *Combatant
	Target   *Combatant
}

type TurnContext struct {
	Target  *Combatant
	Enemies int
}
