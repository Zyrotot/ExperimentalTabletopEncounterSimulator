package entity

type Runtime struct {
	HP        int
	TempHP    int
	TempDR    int
	BonusAC   int
	Stacks    map[string]int
	IsFlanked bool
}
