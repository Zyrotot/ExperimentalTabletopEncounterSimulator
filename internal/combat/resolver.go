package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	logging "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type Combatant struct {
	Char    *entity.Character
	Attacks []Attack
	Effects []Effect
}

type Resolver struct {
	Dice dice.Roller
	Log  *logging.Logger
}

func NewResolver(dice dice.Roller, log *logging.Logger) *Resolver {
	return &Resolver{
		Dice: dice,
		Log:  log,
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

func SumDamage(damageInstances []rules.DamageInstance) int {
	total := 0
	for _, v := range damageInstances {
		total += v.Amount
	}
	return total
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

	for _, eff := range target.Effects {
		eff.On(EventDealHit, &ctx)
	}
}

func (r *Resolver) ResolveCrit(roll int, crit_range int, fortification int) bool {
	if roll >= crit_range {
		fort_roll := r.Dice.Roll(dice.Term{
			Count: 1,
			Sides: 100,
			Flat:  0,
		})
		if fort_roll > fortification {
			return false
		}
		return true
	}
	return false
}

func (r *Resolver) ResolveAttack(attacker, target *Combatant) {
	for _, atk := range attacker.Attacks {
		atkResult := AttackResult{}
		atkResult.AttackRoll = r.Dice.Roll(dice.Term{
			Count: 1,
			Sides: 20,
			Flat:  0,
		})
		atkResult.TotalAtk = atkResult.AttackRoll + atk.AttackBonus
		ac := target.Char.Runtime.AC

		r.Log.Infof("Rolled a %d against %d AC", atkResult.TotalAtk, ac)

		atkResult.Hit = atkResult.TotalAtk >= ac

		if atkResult.Hit {
			r.OnHitEffects(attacker, target)

			atkResult.Damage = make([]rules.DamageInstance, len(atk.DamageDice))

			for i, dmgExp := range atk.DamageDice {
				damage := r.Dice.Roll(dmgExp.DamageRoll)
				atkResult.Damage[i] = rules.DamageInstance{
					Amount: damage,
					Types:  dmgExp.DamageTypes,
				}
			}

			atkResult.Crit = r.ResolveCrit(atkResult.AttackRoll, atk.CritRange, target.Char.Stats.Fort)
			if atkResult.Crit {
				for i, damage := range atkResult.Damage {
					atkResult.Damage[i] = rules.DamageInstance{
						Amount: damage.Amount * atk.CritBonus,
						Types:  damage.Types,
					}
				}
			}

			for _, mod := range atk.DamageModifiers {
				affected, damageType := mod.ModifyDamage(&DamageContext{
					Attacker: attacker,
					Target:   target,
					Damage:   atkResult.Damage,
				})
				if affected {
					r.Log.Infof("Damage modifier applied!")
					atkResult.Damage = append(atkResult.Damage, rules.DamageInstance{
						Amount: r.Dice.Roll(mod.GetTerm()),
						Types:  []rules.DamageType{damageType},
					})
				}
			}

			target.Char.ApplyDR(atkResult.Damage)

			r.OnDamageEffects(attacker, target, atkResult.Damage)

			atkResult.TotalDamage = SumDamage(atkResult.Damage)

			target.Char.TakeDamage(atkResult.TotalDamage)
			r.Log.Infof("%s hits %s for %d damage!", attacker.Char.Name, target.Char.Name, atkResult.TotalDamage)
		} else {
			r.Log.Infof("%s misses %s.", attacker.Char.Name, target.Char.Name)
		}
	}
}
