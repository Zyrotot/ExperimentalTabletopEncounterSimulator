package dice

import (
	"math/rand"
	"regexp"
	"strconv"
)

type Roller interface {
	Roll(dice string) int
}

type RandomRoller struct{}

type FixedRoller struct {
	Value int
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

	total := modifier
	for range numDice {
		total += rand.Intn(diceSides) + 1
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
