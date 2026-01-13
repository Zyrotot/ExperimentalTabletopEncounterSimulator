package combat

import (
	"log"

	"zyrotot.com/ETTES/internal/dice"
	"zyrotot.com/ETTES/internal/entity"
)

type Resolver struct {
	Dice dice.Roller
	Log  *log.Logger
}

func (r *Resolver) OnDamageEffects(attacker, target *entity.Character, ammount *int) {
	for _, e := range target.Effects {
		if eff, ok := e.(OnTakeDamageEffect); ok {

			ctx := DamageContext{attacker, target, ammount}
			eff.OnTakeDamage(&ctx)
		}
	}

	if *ammount > 0 {
		for _, e := range attacker.Effects {
			if eff, ok := e.(OnDealDamageEffect); ok {

				ctx := DamageContext{attacker, target, ammount}
				eff.OnDealDamage(&ctx)
			}
		}
	}
}

func (r *Resolver) OnHitEffects(attacker, target *entity.Character) {
	for _, e := range target.Effects {
		if eff, ok := e.(OnTakeHitEffect); ok {

			ctx := HitContext{attacker, target}
			eff.OnTakeHit(&ctx)
		}
	}

	for _, e := range target.Effects {
		if eff, ok := e.(OnDealHitEffect); ok {

			ctx := HitContext{attacker, target}
			eff.OnDealHit(&ctx)
		}
	}
}

func (r *Resolver) ResolveAttack(attacker, target *entity.Character) {
	for _, atk := range attacker.Attacks {
		roll := r.Dice.Roll("1d20") + atk.AttackBonus
		ac := target.Stats.AC + target.Runtime.BonusAC

		if roll >= ac {

			r.OnHitEffects(attacker, target)

			damage := r.Dice.Roll(atk.DamageDice)

			if roll >= atk.CritRange {
				damage *= atk.CritBonus
			}

			r.OnDamageEffects(attacker, target, &damage)

			target.TakeDamage(damage)
			r.Log.Printf("%s hits %s for %d damage!", attacker.Name, target.Name, damage)
		} else {
			r.Log.Printf("%s misses %s.", attacker.Name, target.Name)
		}
	}
}
