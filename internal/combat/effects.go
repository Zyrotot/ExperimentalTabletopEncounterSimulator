package combat

type OnAttackEffect interface {
	OnAttack(ctx *AttackContext)
}

type OnTakeHitEffect interface {
	OnTakeHit(ctx *HitContext)
}

type OnDealHitEffect interface {
	OnDealHit(ctx *HitContext)
}

type OnTakeDamageEffect interface {
	OnTakeDamage(ctx *DamageContext)
}

type OnDealDamageEffect interface {
	OnDealDamage(ctx *DamageContext)
}

type OnTurnStartEffect interface {
	OnTurnStart(ctx *TurnContext)
}

type VampiricWeapon struct{}

func (VampiricWeapon) OnDealDamage(ctx *DamageContext) {
	heal := *ctx.Damage / 2
	ctx.Attacker.AddTempHP(heal)
}
