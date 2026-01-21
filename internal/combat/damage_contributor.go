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
	Term dice.Term
}

func (e EvilWeapon) Contribute(ctx *CombatContext) rules.DamageInstance {
	if ctx.Target.Char.Alignment.Moral != entity.Good {
		return rules.DamageInstance{}
	}

	return rules.DamageInstance{
		Amount: ctx.Roller.Roll(e.Term),
		Types:  []rules.DamageType{rules.Evil},
	}
}

type FlamingWeapon struct {
	Term dice.Term
}

func (f FlamingWeapon) Contribute(ctx *CombatContext) rules.DamageInstance {
	return rules.DamageInstance{
		Amount: ctx.Roller.Roll(f.Term),
		Types:  []rules.DamageType{rules.Fire},
	}
}
