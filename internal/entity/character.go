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
