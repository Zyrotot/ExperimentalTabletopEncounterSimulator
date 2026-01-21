package combat

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type Effect interface {
	On(event Event, ctx any)
}

type VampiricWeapon struct{}

func (vw *VampiricWeapon) On(event Event, ctx any) {
	if event != EventDealDamage {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	totalDamage := 0
	for _, dmgInst := range dctx.Damage {
		totalDamage += dmgInst.Amount
	}

	heal := totalDamage / 2
	dctx.Attacker.Char.AddTempHP(heal)
}

type DuroDeFerir struct {
	Stacks int
}

func (ddf *DuroDeFerir) On(event Event, ctx any) {
	if event != EventTakeDamage {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	if ddf.Stacks > 0 {
		for i := range dctx.Damage {
			dctx.Damage[i].Amount = 0
		}
		ddf.Stacks--
	}
}

type DuroDeMatar struct {
	Stacks int
}

func (ddm *DuroDeMatar) On(event Event, ctx any) {
	if event != EventTakeDamage {
		return
	}

	dctx, ok := ctx.(*DamageContext)
	if !ok {
		return
	}

	if ddm.Stacks > 0 && rules.SumDamage(dctx.Damage) > dctx.Target.Char.Runtime.GetTotalHP() {
		for i := range dctx.Damage {
			dctx.Damage[i].Amount = 0
		}
		ddm.Stacks--
	}
}

type RigidezRaivosa struct{}

func (rr *RigidezRaivosa) On(event Event, ctx any) {
	switch event {

	case EventTakeDamage:
		dctx, ok := ctx.(*DamageContext)
		if !ok {
			return
		}

		dr := entity.DamageReduction{
			Value:      1,
			BypassType: "",
			Source:     "RigidezRaivosa",
		}
		dctx.Target.Char.AddDR(dr)

	case EventHeal:
		hctx, ok := ctx.(*HealContext)
		if !ok {
			return
		}
		hctx.Target.Char.RemoveDRBySource("RigidezRaivosa")
	}
}

type Erosion struct{}

func (er *Erosion) On(event Event, ctx any) {
	if event != EventDealHit {
		return
	}

	dctx, ok := ctx.(*HitContext)
	if !ok {
		return
	}

	dctx.Target.Char.Runtime.DRSuppressed = append(
		dctx.Target.Char.Runtime.DRSuppressed,
		entity.DRSuppression{
			Source: "Erosion",
			Value:  1,
		},
	)
}

type Cleave struct{}

func (cl *Cleave) On(event Event, ctx any) {
	if event != EventKill {
		return
	}

	// dctx, ok := ctx.(*CombatContext)
	// if !ok {
	// 	return
	// }

	// TODO: Trigger another attack
}
