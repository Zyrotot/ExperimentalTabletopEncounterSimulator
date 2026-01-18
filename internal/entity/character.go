package entity

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

func (c *Character) AddDR(ammount int) {
	c.Runtime.DR += ammount
}

func (c *Character) ApplyDR(damage *int) {
	*damage -= c.Runtime.DR

	if *damage < 0 {
		*damage = 0
	}
}
