package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	logging "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
)

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

func (r *Resolver) OnDamageEffects(attacker, target *Combatant, ammount *int) {
	ctx := DamageContext{attacker, target, ammount}

	for _, eff := range target.Effects {
		eff.On(EventTakeDamage, &ctx)
	}

	if *ammount > 0 {
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
		fort_roll := r.Dice.Roll("1d100")
		if fort_roll > fortification {
			return false
		}
		return true
	}
	return false
}

func (r *Resolver) ResolveAttack(attacker, target *Combatant) {
	for _, atk := range attacker.Char.Attacks {
		roll := r.Dice.Roll("1d20")
		atk_total := roll + atk.AttackBonus
		ac := target.Char.Runtime.AC

		r.Log.Infof("Rolled a %d against %d AC", atk_total, ac)

		if atk_total >= ac {
			r.OnHitEffects(attacker, target)

			damage := r.Dice.Roll(atk.DamageDice)

			if r.ResolveCrit(roll, atk.CritRange, target.Char.Stats.Fort) {
				damage *= atk.CritBonus
			}

			target.Char.ApplyDR(&damage)

			r.OnDamageEffects(attacker, target, &damage)

			target.Char.TakeDamage(damage)
			r.Log.Infof("%s hits %s for %d damage!", attacker.Char.Name, target.Char.Name, damage)
		} else {
			r.Log.Infof("%s misses %s.", attacker.Char.Name, target.Char.Name)
		}
	}
}
