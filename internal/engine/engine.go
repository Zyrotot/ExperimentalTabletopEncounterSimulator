package engine

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"

type Encounter struct {
	Allies  []*combat.Combatant
	Enemies []*combat.Combatant

	Round int
}

type TargetSelector interface {
	Select(attacker *combat.Combatant, candidates []*combat.Combatant) *combat.Combatant
}

type FirstAliveSelector struct{}

func (FirstAliveSelector) Select(_ *combat.Combatant, candidates []*combat.Combatant) *combat.Combatant {
	for _, c := range candidates {
		if !c.Char.IsDead() {
			return c
		}
	}
	return nil
}

type Engine interface {
	Setup(encounter *Encounter)
	Run(encounter *Encounter)
}

type Action interface {
	Execute(r *combat.Resolver)
}

type AutoEngine struct {
	Resolver       *combat.Resolver
	TargetSelector TargetSelector
}

func (ae *AutoEngine) Setup(enc *Encounter) {
	for _, ally := range enc.Allies {
		ally.Char.Runtime.HP = ally.Char.Stats.MaxHP
	}
	for _, enemy := range enc.Enemies {
		enemy.Char.Runtime.HP = enemy.Char.Stats.MaxHP
	}
}

func (ae *AutoEngine) Run(enc *Encounter) {
	for ae.anyAlive(enc.Allies) && ae.anyAlive(enc.Enemies) {
		for _, ally := range enc.Allies {
			if ally.Char.IsDead() {
				continue
			}

			ae.Resolver.ExecuteTurn(
				ally,
				enc.Allies,
				enc.Enemies,
				func(c []*combat.Combatant) *combat.Combatant {
					return ae.TargetSelector.Select(ally, c)
				},
			)
		}

		for _, enemy := range enc.Enemies {
			if enemy.Char.IsDead() {
				continue
			}

			ae.Resolver.ExecuteTurn(
				enemy,
				enc.Enemies,
				enc.Allies,
				func(c []*combat.Combatant) *combat.Combatant {
					return ae.TargetSelector.Select(enemy, c)
				},
			)
		}
		log.Infof("-------- turn ended -----------")
	}
}

func (ae *AutoEngine) anyAlive(combatants []*combat.Combatant) bool {
	for _, char := range combatants {
		if !char.Char.IsDead() {
			return true
		}
	}
	return false
}
