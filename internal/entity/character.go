package entity

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

type MoralAlignment string
type EthicAlignment string

const (
	Good         MoralAlignment = "Good"
	Evil         MoralAlignment = "Evil"
	NeutralMoral MoralAlignment = "Neutral"
	Chaotic      EthicAlignment = "Chaotic"
	Lawful       EthicAlignment = "Lawful"
	NeutralEthic EthicAlignment = "Neutral"
)

type Alignment struct {
	Moral MoralAlignment
	Ethic EthicAlignment
}

type Character struct {
	Name      string
	Stats     Stats
	Runtime   Runtime
	Alignment Alignment
	IsNPC     bool
}

func (c *Character) AddTempHP(ammount int) {
	c.Runtime.TempHP = ammount
}

func (c *Character) TakeDamage(ammount int) {
	if c.Runtime.TempHP > 0 {
		if c.Runtime.TempHP > ammount {
			c.Runtime.TempHP -= ammount
		} else {
			ammount -= c.Runtime.TempHP
			c.Runtime.TempHP = 0
		}
	}

	// TODO: Add Duro de Matar and Ferir stacks (as events?)

	c.Runtime.HP -= ammount
}

func (c *Character) ApplyDR(damage map[rules.DamageType]int) {
	for dmgType, amount := range damage {
		remaining := amount

		for _, dr := range c.Runtime.DR {
			if dr.BypassType == dmgType {
				continue
			}

			if remaining <= 0 {
				break
			}

			reduced := min(remaining, dr.Value)
			remaining -= reduced
		}

		damage[dmgType] = remaining
	}
}

func (c *Character) AddDR(newDR DamageReduction) {
	for i, existing := range c.Runtime.DR {
		if existing.BypassType == newDR.BypassType {
			c.Runtime.DR[i].Value += newDR.Value
			return
		}
	}

	c.Runtime.DR = append(c.Runtime.DR, newDR)
}
