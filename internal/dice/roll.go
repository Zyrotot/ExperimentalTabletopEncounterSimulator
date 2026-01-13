package dice

type Roller interface {
	Roll(dice string) int
}

type RandomRoller struct{}

type FixedRoller struct {
	Value int
}
