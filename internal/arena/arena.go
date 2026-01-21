package arena

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"

type Arena interface {
	Apply(ctx *combat.TurnContext)
}

type OpenField struct{}
type AgainstWall struct{}
type AgainstCorner struct{}

// func (a *OpenField) Apply(ctx *combat.TurnContext) {
// 	if *ctx.Enemies > 8 {
// 		*ctx.Enemies = 8
// 	}
// 	if *ctx.Enemies >= 2 {
// 		ctx.Target.Char.Runtime.IsFlanked = true
// 	} else {
// 		ctx.Target.Char.Runtime.IsFlanked = false
// 	}
// }

// func (a *AgainstWall) Apply(ctx *combat.TurnContext) {
// 	if *ctx.Enemies > 5 {
// 		*ctx.Enemies = 5
// 	}
// 	if *ctx.Enemies >= 2 {
// 		ctx.Target.Char.Runtime.IsFlanked = true
// 	} else {
// 		ctx.Target.Char.Runtime.IsFlanked = false
// 	}
// }

// func (a *AgainstCorner) Apply(ctx *combat.TurnContext) {
// 	if *ctx.Enemies > 3 {
// 		*ctx.Enemies = 3
// 	}
// 	ctx.Target.Char.Runtime.IsFlanked = false
// }
