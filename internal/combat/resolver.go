package combat

import (
	"log"
	"os"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
)

type Resolver struct {
	Dice dice.Roller
	Log  *log.Logger
}

func NewResolver(dice dice.Roller) *Resolver {
	return &Resolver{
		Dice: dice,
		Log:  log.New(os.Stdout, "[combat] ", log.LstdFlags),
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

func (r *Resolver) ResolveAttack(attacker, target *Combatant) {
	for _, atk := range attacker.Char.Attacks {
		roll := r.Dice.Roll("1d20")
		atk_total := roll + atk.AttackBonus
		ac := target.Char.Runtime.AC

		r.Log.Printf("Rolled a %d against %d AC", atk_total, ac)

		if atk_total >= ac {
			r.OnHitEffects(attacker, target)

			damage := r.Dice.Roll(atk.DamageDice)

			if roll >= atk.CritRange {
				damage *= atk.CritBonus
			}

			target.Char.ApplyDR(&damage)

			r.OnDamageEffects(attacker, target, &damage)

			target.Char.TakeDamage(damage)
			r.Log.Printf("%s hits %s for %d damage!", attacker.Char.Name, target.Char.Name, damage)
		} else {
			r.Log.Printf("%s misses %s.", attacker.Char.Name, target.Char.Name)
		}
	}
}
