package combat

type Effect interface {
	On(event Event, ctx any)
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

	totalDamage := 0
	for _, dmg := range dctx.Damage {
		totalDamage += dmg
	}

	heal := totalDamage / 2
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
