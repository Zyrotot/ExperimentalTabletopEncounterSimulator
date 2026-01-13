package arena

import "zyrotot.com/ETTES/internal/combat"

type Arena interface {
	Apply(ctx *combat.TurnContext)
	MaxAttackers() int
}
