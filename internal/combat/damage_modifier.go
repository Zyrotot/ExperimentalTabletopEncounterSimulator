package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type DamageModifier interface {
	ModifyDamage(ctx *DamageContext) (bool, rules.DamageType)
	GetTerm() dice.Term
}

type BaseDamageModifier struct {
	Term dice.Term
}

func (e BaseDamageModifier) GetTerm() dice.Term {
	return e.Term
}

type EvilWeapon struct {
	BaseDamageModifier
}

func (e EvilWeapon) ModifyDamage(ctx *DamageContext) (bool, rules.DamageType) {
	if ctx.Target.Char.Alignment.Moral != entity.Good {
		return false, ""
	}

	return true, rules.Evil
}

type FlammingWeapon struct {
	BaseDamageModifier
}

func (f FlammingWeapon) ModifyDamage(ctx *DamageContext) (bool, rules.DamageType) {
	return true, rules.Fire
}
