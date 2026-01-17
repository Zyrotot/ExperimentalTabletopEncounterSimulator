package dice

import (
	"math/rand"
	"regexp"
	"strconv"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
)

type Roller interface {
	Roll(dice string) int
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

func (r *RandomRoller) Roll(dice string) int {
	re := regexp.MustCompile(`(\d+)d(\d+)([+-]\d+)?`)
	match := re.FindStringSubmatch(dice)
	if match == nil {
		return 0
	}
	numDice, _ := strconv.Atoi(match[1])
	diceSides, _ := strconv.Atoi(match[2])
	modifier := 0
	if len(match) > 3 {
		modifier, _ = strconv.Atoi(match[3])
	}

	r.Log.Infof("Rolling %d dice of %d sides with a bonus of %d", numDice, diceSides, modifier)

	total := modifier
	for range numDice {
		rolled_dice := rand.Intn(diceSides) + 1
		total += rolled_dice
		r.Log.Debugf("Rolled a %d", rolled_dice)
	}
	return total
}

func (f *FixedRoller) Roll(dice string) int {
	re := regexp.MustCompile(`(\d+)d(\d+)([+-]\d+)?`)
	match := re.FindStringSubmatch(dice)
	if match == nil {
		return 0
	}
	numDice, _ := strconv.Atoi(match[1])
	diceSides, _ := strconv.Atoi(match[2])
	modifier := 0
	if len(match) > 3 {
		modifier, _ = strconv.Atoi(match[3])
	}
	total := modifier
	for range numDice {
		total += diceSides
	}
	return total
}
