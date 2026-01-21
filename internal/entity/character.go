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

func (c *Character) ApplyResistances(damage []rules.DamageInstance) {
	c.ApplyImmunities(damage)
	c.ApplyDR(damage)
	c.ApplyER(damage)
}

func (im Immunity) Affects(d rules.DamageInstance) bool {
	if im.Type != "" && d.HasType(im.Type) {
		return true
	}
	if im.Category != 0 && d.HasCategory(im.Category) {
		return true
	}
	return false
}

func (c *Character) ApplyImmunities(damage []rules.DamageInstance) {
	for i := range damage {
		for _, im := range c.Runtime.Immunities {
			if im.Affects(damage[i]) {
				log.Infof("Immunity applied")
				damage[i].Amount = 0
				break
			}
		}
	}
}

func (dr DamageReduction) IsBypassedBy(d rules.DamageInstance) bool {
	if !d.IsPhysical() {
		log.Infof("Not physical, ignoring")
		return true
	}
	for _, t := range d.Types {
		if t == dr.BypassType {
			log.Infof("Has bypass type, ignoring")
			return true
		}
	}
	return false
}

func (c *Character) ApplyDR(damage []rules.DamageInstance) {
	for _, dr := range c.Runtime.DR {
		remaining := dr.Value

		for i := range damage {
			if remaining <= 0 {
				break
			}

			if dr.IsBypassedBy(damage[i]) {
				continue
			}

			reduced := min(damage[i].Amount, remaining)
			log.Infof("DR - Reduced by %d", reduced)

			damage[i].Amount -= reduced
			remaining -= reduced
		}
	}
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

func (er EnergyResistance) Affects(d rules.DamageInstance) bool {
	if !d.IsEnergy() {
		log.Infof("Not energy, ignoring")
		return false
	}

	if er.AffectedType == "" {
		return true
	}

	return d.HasType(er.AffectedType)
}

func (c *Character) ApplyER(damage []rules.DamageInstance) {
	for _, er := range c.Runtime.ER {
		remaining := er.Value

		for i := range damage {
			if remaining <= 0 {
				break
			}

			if !er.Affects(damage[i]) {
				continue
			}

			reduced := min(damage[i].Amount, remaining)
			log.Infof("ER - Reduced by %d", reduced)

			damage[i].Amount -= reduced
			remaining -= reduced
		}
	}
}

func (c *Character) Heal(amount int) {
	c.Runtime.HP = min(c.Runtime.HP+amount, c.Stats.MaxHP)
}
