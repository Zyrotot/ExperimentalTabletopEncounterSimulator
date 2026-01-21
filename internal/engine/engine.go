package engine

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"

type Encounter struct {
	Allies  []*combat.Combatant
	Enemies []*combat.Combatant

	Round int
}

type Engine interface {
	Run(encounter *Encounter)
}

type Action interface {
	Execute(r *combat.Resolver)
}

type AttackAction struct {
	Attacker *combat.Combatant
	Attack   combat.Attack
	Target   *combat.Combatant
}

func (a AttackAction) Execute(r *combat.Resolver) {
	r.ResolveAttack(a.Attacker, a.Target, a.Attack)
}

type AutoEngine struct {
	Resolver *combat.Resolver
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
			target := ae.firstAlive(enc.Enemies)
			if target == nil {
				return
			}
			for _, atk := range ally.Attacks {
				AttackAction{ally, atk, target}.Execute(ae.Resolver)
			}
			ae.resolvePending(ally, enc.Allies, enc.Enemies)
		}

		for _, enemy := range enc.Enemies {
			if enemy.Char.IsDead() {
				continue
			}
			target := ae.firstAlive(enc.Allies)
			if target == nil {
				return
			}
			for _, atk := range enemy.Attacks {
				AttackAction{enemy, atk, target}.Execute(ae.Resolver)
			}
			ae.resolvePending(enemy, enc.Enemies, enc.Allies)
		}
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

func (ae *AutoEngine) firstAlive(combatants []*combat.Combatant) *combat.Combatant {
	for _, char := range combatants {
		if !char.Char.IsDead() {
			return char
		}
	}
	return nil
}

func (e *AutoEngine) resolvePending(actor *combat.Combatant, allies, enemies []*combat.Combatant) {
	for len(actor.PendingActions) > 0 {
		req := actor.PendingActions[0]
		actor.PendingActions = actor.PendingActions[1:]

		switch req := req.(type) {
		case combat.ExtraAttackRequest:
			target := e.firstAlive(enemies)
			if target != nil {
				AttackAction{req.Source, req.Attack, target}.Execute(e.Resolver)
			}
		}
	}
}
