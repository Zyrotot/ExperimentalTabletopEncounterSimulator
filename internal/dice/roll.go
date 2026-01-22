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

type RandomRoller struct {
}

type FixedRoller struct {
	Value int
}

func NewRandomRoller() *RandomRoller {
	return &RandomRoller{}
}

func NewFixedRoller(value int, log *logging.Logger) *FixedRoller {
	return &FixedRoller{
		Value: value,
	}
}

func (r *RandomRoller) Roll(term Term) int {
	log.Debugf("Rolling %d dice of %d sides with a bonus of %d", term.Count, term.Sides, term.Flat)

	total := term.Flat
	for range term.Count {
		rolled_dice := rand.Intn(term.Sides) + 1
		total += rolled_dice
		log.Debugf("Rolled a %d", rolled_dice)
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
