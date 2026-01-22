package combat

type ActionRequest interface {
}

type ExtraAttackRequest struct {
	Source *Combatant
	Attack Attack
	Reason string
}
