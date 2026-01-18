package dice

import (
	"math/rand"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
)

type Term struct {
	Count int
	Sides int
	Flat  int
}

type Expression struct {
	Terms []Term
}

type Roller interface {
	Roll(term Term) int
}

type BaseRoller struct {
	Log *logging.Logger
}

type RandomRoller struct {
	BaseRoller
}

type FixedRoller struct {
	BaseRoller
	Value int
}

func NewRandomRoller(log *logging.Logger) *RandomRoller {
	return &RandomRoller{
		BaseRoller: BaseRoller{
			Log: log,
		},
	}
}

func NewFixedRoller(value int, log *logging.Logger) *FixedRoller {
	return &FixedRoller{
		BaseRoller: BaseRoller{
			Log: log,
		},
		Value: value,
	}
}

func (r *RandomRoller) Roll(term Term) int {
	r.Log.Infof("Rolling %d dice of %d sides with a bonus of %d", term.Count, term.Sides, term.Flat)

	total := term.Flat
	for range term.Count {
		rolled_dice := rand.Intn(term.Sides) + 1
		total += rolled_dice
		r.Log.Debugf("Rolled a %d", rolled_dice)
	}
	return total
}

func (f *FixedRoller) Roll(term Term) int {
	total := term.Flat
	for range term.Count {
		total += term.Sides
	}
	return total
}
