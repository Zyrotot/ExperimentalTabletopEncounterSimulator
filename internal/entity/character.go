package entity

import (
	"sort"

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
	if c.Runtime.TempHP < ammount {
		c.Runtime.TempHP = ammount
	}
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

	c.Runtime.HP -= ammount
}

func (c *Character) DamageModifiers() []rules.DamageModifier {
	var mods []rules.DamageModifier

	for _, wk := range c.Runtime.Weaknesses {
		mods = append(mods, wk)
	}
	for _, im := range c.Runtime.Immunities {
		mods = append(mods, im)
	}
	for _, dr := range c.Runtime.DR {
		mods = append(mods, dr)
	}
	for _, er := range c.Runtime.ER {
		mods = append(mods, er)
	}

	sort.Slice(mods, func(i, j int) bool {
		return mods[i].Priority() < mods[j].Priority()
	})

	return mods
}

func (c *Character) AddDR(newDR DamageReduction) {
	for i, existing := range c.Runtime.DR {
		if existing.BypassType == newDR.BypassType &&
			existing.Source == newDR.Source {
			c.Runtime.DR[i].Value += newDR.Value
			return
		}
	}
	c.Runtime.DR = append(c.Runtime.DR, newDR)
}

func (c *Character) RemoveDRBySource(source string) {
	filtered := c.Runtime.DR[:0]
	for _, dr := range c.Runtime.DR {
		if dr.Source != source {
			filtered = append(filtered, dr)
		}
	}
	c.Runtime.DR = filtered
}

func (c *Character) Heal(amount int) {
	c.Runtime.HP = min(c.Runtime.HP+amount, c.Stats.MaxHP)
}
