package combat

import "zyrotot.com/ETTES/internal/entity"

type Effect interface {
	On(event Event, ctx any)
}

type Combatant struct {
	Char    *entity.Character
	Effects []Effect
}

type VampiricWeapon struct{}

func (VampiricWeapon) On(event Event, ctx any) {
	if event != "deal_damage" {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	heal := *dctx.Damage / 2
	dctx.Attacker.Char.AddTempHP(heal)
}
