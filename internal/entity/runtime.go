package entity

type Runtime struct {
	HP        int
	TempHP    int
	TempDR    int
	Stacks    map[string]int
	IsFlanked bool
}
