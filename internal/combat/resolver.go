package combat

import (
	"log"

	"zyrotot.com/ETTES/internal/dice"
	"zyrotot.com/ETTES/internal/entity"
)

type Resolver struct {
	Dice dice.Roller
	Log  log.Logger
}

func (r *Resolver) ResolveAttack(attacker, target *entity.Character)
