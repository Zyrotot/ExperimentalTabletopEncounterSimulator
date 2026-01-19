package entity

type Runtime struct {
	HP        int
	TempHP    int
	DR        []DamageReduction
	ER        []EnergyResistance
	AC        int
	Stacks    map[string]int
	IsFlanked bool
}

func (rt *Runtime) GetTotalHP() int {
	return rt.HP + rt.TempHP
}
