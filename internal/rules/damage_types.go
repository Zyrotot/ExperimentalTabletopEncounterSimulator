package rules

type DamageType string

const (
	Slash    DamageType = "slash"
	Pierce   DamageType = "pierce"
	Blunt    DamageType = "blunt"
	Magic    DamageType = "magic"
	Fire     DamageType = "fire"
	Cold     DamageType = "cold"
	Acid     DamageType = "acid"
	Electric DamageType = "electric"
	Silver   DamageType = "silver"
	Evil     DamageType = "evil"
	Good     DamageType = "good"
	Chaotic  DamageType = "chaotic"
	Lawful   DamageType = "lawful"
)
