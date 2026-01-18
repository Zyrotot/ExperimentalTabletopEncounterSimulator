package entity

type Runtime struct {
	HP        int
	TempHP    int
	DR        []DamageReduction
	AC        int
	Stacks    map[string]int
	IsFlanked bool
}
