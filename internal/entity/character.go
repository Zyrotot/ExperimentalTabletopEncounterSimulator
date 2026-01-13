package entity

type Attack struct {
	Name        string `json:"name"`
	DamageDice  string `json:"damageDice"`
	AttackBonus int    `json:"attackBonus"`
	CritRange   int    `json:"critRange"`
	CritBonus   int    `json:"critBonus"`
}

type Character struct {
	Name    string
	Stats   Stats
	Runtime Runtime
	Attacks []Attack
	Effects []any
	IsNPC   bool
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
	c.Runtime.HP -= ammount
}

func (c *Character) AddDR(ammount int) {
	c.Runtime.TempDR += ammount
}
