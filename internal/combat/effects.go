package combat

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"

type Effect interface {
	On(event Event, ctx any)
}

type Combatant struct {
	Char    *entity.Character
	Effects []Effect
}

type VampiricWeapon struct{}

func (VampiricWeapon) On(event Event, ctx any) {
	if event != EventDealDamage {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	heal := *dctx.Damage / 2
	dctx.Attacker.Char.AddTempHP(heal)
}

type RigidezRaivosa struct{}

func (RigidezRaivosa) On(event Event, ctx any) {
	if event != EventTakeDamage {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	dctx.Target.Char.AddDR(1)
}
