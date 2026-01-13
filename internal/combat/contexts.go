package combat

import (
	"zyrotot.com/ETTES/internal/entity"
)

type AttackContext struct {
	Attacker *entity.Character
	Target   *entity.Character
	Roll     int
	Hit      bool
}

type DamageContext struct {
	Attacker *entity.Character
	Target   *entity.Character
	Damage   *int
}

type HitContext struct {
	Attacker *entity.Character
	Target   *entity.Character
}

type TurnContext struct {
	Target  *entity.Character
	Enemies int
}
