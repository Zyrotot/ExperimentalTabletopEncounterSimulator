package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type Combatant struct {
	Char    *entity.Character
	Attacks []Attack
	Effects []Effect

	PendingActions []ActionRequest
}

type Resolver struct {
	Dice dice.Roller
}

func NewResolver(dice dice.Roller) *Resolver {
	return &Resolver{
		Dice: dice,
	}
}

func hasPositiveDamage(damageInstances []rules.DamageInstance) bool {
	for _, v := range damageInstances {
		if v.Amount > 0 {
			return true
		}
	}
	return false
}

func (r *Resolver) OnDamageEffects(attacker, target *Combatant, damageInstances []rules.DamageInstance) {
	ctx := DamageContext{
		Attacker: attacker,
		Target:   target,
		Damage:   damageInstances,
	}

	for _, eff := range target.Effects {
		eff.On(EventTakeDamage, &ctx)
	}

	if hasPositiveDamage(damageInstances) {
		for _, eff := range attacker.Effects {
			eff.On(EventDealDamage, &ctx)
		}
	}
}

func (r *Resolver) OnHitEffects(attacker, target *Combatant) {
	ctx := HitContext{attacker, target}
	for _, eff := range target.Effects {
		eff.On(EventTakeHit, &ctx)
	}

	for _, eff := range attacker.Effects {
		eff.On(EventDealHit, &ctx)
	}
}

func (r *Resolver) Heal(source, target *Combatant, amount int) {
	if amount <= 0 {
		return
	}

	target.Char.Heal(amount)

	ctx := HealContext{
		Source: source,
		Target: target,
		Amount: amount,
	}

	for _, eff := range target.Effects {
		eff.On(EventHeal, &ctx)
	}
}

func (r *Resolver) ResolveCrit(roll int, crit_range int, fortification int) bool {
	if roll >= crit_range {
		fort_roll := r.Dice.Roll(dice.Term{
			Count: 1,
			Sides: 100,
			Flat:  0,
		})
		log.Infof("Fortification roll: %d against %d", fort_roll, fortification)
		if fort_roll > fortification {
			return true
		}
		return false
	}
	return false
}

func (r *Resolver) ApplyDamageMitigators(target *Combatant, dmg []rules.DamageInstance) {
	for _, mod := range target.Char.DamageMitigators() {
		mod.Apply(dmg)
	}
}

func (r *Resolver) ExecuteAttack(attacker, target *Combatant, atk Attack) {
	atkResult := AttackResult{Attack: atk}
	atkResult.AttackRoll = r.Dice.Roll(dice.Term{
		Count: 1,
		Sides: 20,
		Flat:  0,
	})
	atkResult.TotalAtk = atkResult.AttackRoll + atk.AttackBonus
	ac := target.Char.GetTotalAC()

	log.Infof("Rolled a %d against %d AC", atkResult.TotalAtk, ac)

	atkResult.Hit = (atkResult.AttackRoll == 20) || (atkResult.TotalAtk >= ac)

	if atkResult.Hit {
		r.OnHitEffects(attacker, target)

		atkResult.Damage = make([]rules.DamageInstance, len(atk.DamageDice))

		for i, dmgExp := range atk.DamageDice {
			damage := 0
			for _, term := range dmgExp.DamageRoll.Terms {
				damage += r.Dice.Roll(term)
			}
			log.Debugf("Calculated damage for expression %d: %d", i, damage)
			atkResult.Damage[i] = rules.DamageInstance{
				Amount: damage,
				Types:  dmgExp.DamageTypes,
			}
		}

		atkResult.Crit = r.ResolveCrit(atkResult.AttackRoll, atk.CritRange, target.Char.Stats.Fort)
		if atkResult.Crit {
			for i, damage := range atkResult.Damage {
				log.Debugf("Critical hit! Multiplying damage instance %d by %d", i, atk.CritBonus)
				atkResult.Damage[i] = rules.DamageInstance{
					Amount: damage.Amount * atk.CritBonus,
					Types:  damage.Types,
				}
			}
		}

		for _, mod := range atk.DamageSources {
			extraDamage := mod.Contribute(&CombatContext{
				Attacker: attacker,
				Target:   target,
				Attack:   atkResult,
				Roller:   r.Dice,
			})
			if extraDamage.Amount > 0 {
				log.Infof("Extra damage instance: %+v", extraDamage)
				atkResult.Damage = append(atkResult.Damage, extraDamage)
			}
		}

		r.ApplyDamageMitigators(target, atkResult.Damage)

		r.OnDamageEffects(attacker, target, atkResult.Damage)

		atkResult.TotalDamage = rules.SumDamage(atkResult.Damage)

		target.Char.TakeDamage(atkResult.TotalDamage)
		log.Infof("%s hits %s for %d damage!", attacker.Char.Name, target.Char.Name, atkResult.TotalDamage)

		killed := target.Char.IsDead()
		if killed {
			ctx := CombatContext{
				Attacker: attacker,
				Target:   target,
			}
			for _, eff := range attacker.Effects {
				eff.On(EventKill, &ctx)
			}
		}
	} else {
		log.Infof("%s misses %s.", attacker.Char.Name, target.Char.Name)
	}
	log.Infof("----- Attack ended -----")
}

func (r *Resolver) StartTurn(c *Combatant) {
	ctx := TurnContext{Combatant: c}
	for _, eff := range c.Effects {
		eff.On(EventTurnStart, &ctx)
	}
}

func (r *Resolver) ExecuteTurn(actor *Combatant, allies []*Combatant, enemies []*Combatant, selectTarget func([]*Combatant) *Combatant) {
	r.StartTurn(actor)

	for _, atk := range actor.Attacks {
		target := selectTarget(enemies)
		if target == nil {
			return
		}
		r.ExecuteAttack(actor, target, atk)
	}

	r.resolvePending(actor, allies, enemies, selectTarget)
}

func (r *Resolver) resolvePending(actor *Combatant, allies, enemies []*Combatant, selectTarget func([]*Combatant) *Combatant) {
	for len(actor.PendingActions) > 0 {
		req := actor.PendingActions[0]
		actor.PendingActions = actor.PendingActions[1:]

		switch req := req.(type) {
		case ExtraAttackRequest:
			target := selectTarget(enemies)
			if target != nil {
				r.ExecuteAttack(req.Source, target, req.Attack)
			}
		}
	}
}
