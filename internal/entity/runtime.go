package entity

type Runtime struct {
	HP        int
	TempHP    int
	DR        int
	AC        int
	Stacks    map[string]int
	IsFlanked bool
}
