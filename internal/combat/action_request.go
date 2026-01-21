package combat

type ActionRequest interface {
	Kind() string
}

type ExtraAttackRequest struct {
	Source *Combatant
	Attack Attack
	Reason string
}

func (e ExtraAttackRequest) Kind() string {
	return "ExtraAttack"
}
