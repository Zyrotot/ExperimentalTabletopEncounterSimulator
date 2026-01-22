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

func (c *Character) allWeaknesses() []Weakness {
	return append(
		append([]Weakness{}, c.Stats.Weaknesses...),
		c.Runtime.AddedWeaknesses...,
	)
}

func (c *Character) allImmunities() []Immunity {
	return append(
		append([]Immunity{}, c.Stats.Immunities...),
		c.Runtime.AddedImmunities...,
	)
}

func (c *Character) allDR() []DamageReduction {
	return append(
		append([]DamageReduction{}, c.Stats.DR...),
		c.Runtime.AddedDR...,
	)
}

func (c *Character) effectiveDR() []DamageReduction {
	drs := c.allDR()

	remainingSuppression := 0
	for _, sup := range c.Runtime.DRSuppressed {
		remainingSuppression += sup.Value
	}

	for i := range drs {
		if remainingSuppression <= 0 {
			break
		}

		reduced := min(drs[i].Value, remainingSuppression)
		drs[i].Value -= reduced
		remainingSuppression -= reduced
	}

	out := drs[:0]
	for _, dr := range drs {
		if dr.Value > 0 {
			out = append(out, dr)
		}
	}

	return out
}

func (c *Character) allER() []EnergyResistance {
	return append(
		append([]EnergyResistance{}, c.Stats.ER...),
		c.Runtime.AddedER...,
	)
}

func (c *Character) DamageMitigators() []rules.DamageMitigator {
	var mods []rules.DamageMitigator

	for _, wk := range c.allWeaknesses() {
		mods = append(mods, wk)
	}
	for _, im := range c.allImmunities() {
		mods = append(mods, im)
	}
	for _, er := range c.allER() {
		mods = append(mods, er)
	}
	for _, dr := range c.effectiveDR() {
		mods = append(mods, dr)
	}

	sort.Slice(mods, func(i, j int) bool {
		return mods[i].Priority() < mods[j].Priority()
	})

	return mods
}

func (c *Character) AddDR(newDR DamageReduction) {
	for i, existing := range c.allDR() {
		if existing.BypassType == newDR.BypassType &&
			existing.Source == newDR.Source {
			c.Runtime.AddedDR[i].Value += newDR.Value
			return
		}
	}
	c.Runtime.AddedDR = append(c.Runtime.AddedDR, newDR)
}

func (c *Character) RemoveDRBySource(source string) {
	filtered := c.Runtime.AddedDR[:0]
	for _, dr := range c.Runtime.AddedDR {
		if dr.Source != source {
			filtered = append(filtered, dr)
		}
	}
	c.Runtime.AddedDR = filtered
}

func (c *Character) Heal(amount int) {
	c.Runtime.HP = min(c.Runtime.HP+amount, c.Stats.MaxHP)
}

func (c *Character) GetTotalAC() int {
	return c.Stats.AC + c.Runtime.ACBonus
}

func (c *Character) IsDead() bool {
	return c.Runtime.HP <= 0
}
