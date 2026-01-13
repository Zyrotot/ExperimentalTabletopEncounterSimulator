package combat

type OnAttackEffect interface {
	OnAttack(ctx *AttackContext)
}

type OnHitEffect interface {
	OnHit(ctx *HitContext)
}

type OnDamageEffect interface {
	OnDamage(ctx *DamageContext)
}

type OnTurnStartEffect interface {
	OnTurnStart(ctx *TurnContext)
}

type VampiricWeapon struct{}

func (VampiricWeapon) OnHit(ctx *DamageContext) {
	heal := *ctx.Damage / 2
	ctx.Attacker.AddTempHP(heal)
}
