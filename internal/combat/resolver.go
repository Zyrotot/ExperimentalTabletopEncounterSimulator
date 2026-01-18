package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	logging "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
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

func hasPositiveDamage(damage map[DamageType]int) bool {
	for _, v := range damage {
		if v > 0 {
			return true
		}
	}
	return false
}

func (r *Resolver) OnDamageEffects(attacker, target *Combatant, ammount map[DamageType]int) {
	ctx := DamageContext{
		Attacker: attacker,
		Target:   target,
		Damage:   ammount,
	}

	for _, eff := range target.Effects {
		eff.On(EventTakeDamage, &ctx)
	}

	if hasPositiveDamage(ammount) {
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
		atkResult.Damage = make(map[DamageType]int)
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

			for dmgType, dmg := range atk.DamageDice.Components {
				damage := r.Dice.Roll(dmg)
				atkResult.Damage[dmgType] += damage
			}

			atkResult.Crit = r.ResolveCrit(atkResult.AttackRoll, atk.CritRange, target.Char.Stats.Fort)
			if atkResult.Crit {
				for _, damage := range atkResult.Damage {
					damage *= atk.CritBonus
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
					atkResult.Damage[damageType] += r.Dice.Roll(mod.GetTerm())
				}
			}

			// target.Char.ApplyDR(&damage)

			r.OnDamageEffects(attacker, target, atkResult.Damage)

			for _, damage := range atkResult.Damage {
				atkResult.TotalDamage += damage
			}

			target.Char.TakeDamage(atkResult.TotalDamage)
			r.Log.Infof("%s hits %s for %d damage!", attacker.Char.Name, target.Char.Name, atkResult.TotalDamage)
		} else {
			r.Log.Infof("%s misses %s.", attacker.Char.Name, target.Char.Name)
		}
	}
}
