package engine

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/arena"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
)

type Encounter struct {
	Arena   arena.Arena
	Allies  []*combat.Combatant
	Enemies []*combat.Combatant

	Round int
}

type TargetSelector interface {
	Select(attacker *combat.Combatant, candidates []*combat.Combatant) *combat.Combatant
}

type FirstAliveSelector struct {
	Tracker *arena.EngagementTracker
}

func (fs *FirstAliveSelector) Select(source *combat.Combatant, candidates []*combat.Combatant) *combat.Combatant {
	for _, c := range candidates {
		if !c.Char.IsDead() && fs.Tracker.CanEngage(source, c) {
			fs.Tracker.Engage(source, c)
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
	if s, ok := ae.TargetSelector.(*FirstAliveSelector); ok { // TODO: Improve this
		s.Tracker = arena.NewEngagementTracker(enc.Arena)
	} else {
		panic("TargetSelector does not support engagement tracking")
	}

	for _, ally := range enc.Allies {
		ally.Char.Runtime.HP = ally.Char.Stats.MaxHP
	}
	for _, enemy := range enc.Enemies {
		enemy.Char.Runtime.HP = enemy.Char.Stats.MaxHP
	}
}

func (ae *AutoEngine) Run(enc *Encounter) {
	log.Infof("Encounter started with %d allies and %d enemies", len(enc.Allies), len(enc.Enemies))
	ae.Setup(enc)
	for ae.anyAlive(enc.Allies) && ae.anyAlive(enc.Enemies) {
		log.Infof("----- Turn started -----")
		if s, ok := ae.TargetSelector.(*FirstAliveSelector); ok { // TODO: Improve this
			s.Tracker.ResetTurn()
		}

		for _, ally := range enc.Allies {
			if ally.Char.IsDead() {
				continue
			}

			ae.Resolver.ExecuteTurn(
				ally,
				enc.Allies,
				enc.Enemies,
				func(source *combat.Combatant, c []*combat.Combatant) *combat.Combatant {
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
				func(source *combat.Combatant, c []*combat.Combatant) *combat.Combatant {
					return ae.TargetSelector.Select(enemy, c)
				},
			)
		}
		log.Infof("----- Turn ended -----")
	}
	log.Infof("Encounter ended")
}

func (ae *AutoEngine) anyAlive(combatants []*combat.Combatant) bool {
	for _, char := range combatants {
		if !char.Char.IsDead() {
			return true
		}
	}
	return false
}
