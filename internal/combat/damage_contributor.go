package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type DamageContributor interface {
	Contribute(ctx *CombatContext) rules.DamageInstance
}

type EvilWeapon struct {
}

func (e EvilWeapon) Contribute(ctx *CombatContext) rules.DamageInstance {
	if ctx.Target.Char.Alignment.Moral != entity.Good {
		return rules.DamageInstance{}
	}

	return rules.DamageInstance{
		Amount: ctx.Roller.Roll(dice.Term{
			Count: 2,
			Sides: 6,
		}),
		Types: []rules.DamageType{rules.Evil},
	}
}

type FlamingWeapon struct {
}

func (f FlamingWeapon) Contribute(ctx *CombatContext) rules.DamageInstance {
	return rules.DamageInstance{
		Amount: ctx.Roller.Roll(dice.Term{
			Count: 1,
			Sides: 6,
		}),
		Types: []rules.DamageType{rules.Fire},
	}
}

type FlamingExplosionWeapon struct {
}

func (fe FlamingExplosionWeapon) Contribute(ctx *CombatContext) rules.DamageInstance {
	damageValue := ctx.Roller.Roll(dice.Term{
		Count: 1,
		Sides: 6,
	})
	if ctx.Attack.Crit {
		dices := min(ctx.Attack.Attack.CritBonus-1, 3)
		damageValue += ctx.Roller.Roll(dice.Term{
			Count: dices,
			Sides: 10,
		})
	}
	return rules.DamageInstance{
		Amount: damageValue,
		Types:  []rules.DamageType{rules.Fire},
	}

}
