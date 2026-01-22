package combat

type Event string

const (
	EventAttack     Event = "attack"
	EventDealHit    Event = "deal_hit"
	EventTakeHit    Event = "take_hit"
	EventDealDamage Event = "deal_damage"
	EventTakeDamage Event = "take_damage"
	EventHeal       Event = "heal"
	EventTurnStart  Event = "turn_start"
	EventKill       Event = "kill"
)
