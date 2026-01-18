package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
)

type DamageModifier interface {
	ModifyDamage(ctx *DamageContext) (bool, DamageType)
	GetTerm() dice.Term
}

type BaseDamageModifier struct {
	Term dice.Term
}

type EvilVsGood struct {
	BaseDamageModifier
}

func (e EvilVsGood) ModifyDamage(ctx *DamageContext) (bool, DamageType) {
	if ctx.Target.Char.Alignment.Moral != entity.Good {
		return false, ""
	}

	return true, Evil
}

func (e BaseDamageModifier) GetTerm() dice.Term {
	return e.Term
}
