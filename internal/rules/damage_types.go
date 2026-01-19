package rules

type DamageType string

const (
	// Physical
	Slash  DamageType = "slash"
	Pierce DamageType = "pierce"
	Blunt  DamageType = "blunt"

	// Energy
	Fire     DamageType = "fire"
	Cold     DamageType = "cold"
	Acid     DamageType = "acid"
	Electric DamageType = "electric"

	// Modifier
	Magic   DamageType = "magic"
	Silver  DamageType = "silver"
	Evil    DamageType = "evil"
	Good    DamageType = "good"
	Chaotic DamageType = "chaotic"
	Lawful  DamageType = "lawful"
)

type DamageCategory int

const (
	Physical DamageCategory = iota
	Energy
	Modifier
)

var damageTypeCategory = map[DamageType]DamageCategory{
	Slash:  Physical,
	Pierce: Physical,
	Blunt:  Physical,

	Fire:     Energy,
	Cold:     Energy,
	Acid:     Energy,
	Electric: Energy,

	Magic:   Modifier,
	Silver:  Modifier,
	Good:    Modifier,
	Evil:    Modifier,
	Lawful:  Modifier,
	Chaotic: Modifier,
}

func (dt DamageType) Category() DamageCategory {
	return damageTypeCategory[dt]
}

func (dt DamageType) IsPhysical() bool {
	return dt.Category() == Physical
}

func (dt DamageType) IsEnergy() bool {
	return dt.Category() == Energy
}

func (dt DamageType) IsModifier() bool {
	return dt.Category() == Modifier
}

type DamageInstance struct {
	Amount int
	Types  []DamageType
}

func SumDamage(damageInstances []DamageInstance) int {
	total := 0
	for _, v := range damageInstances {
		total += v.Amount
	}
	return total
}

func (d DamageInstance) HasType(t DamageType) bool {
	for _, dt := range d.Types {
		if dt == t {
			return true
		}
	}
	return false
}

func (d DamageInstance) HasCategory(cat DamageCategory) bool {
	for _, dt := range d.Types {
		if dt.Category() == cat {
			return true
		}
	}
	return false
}

func (d DamageInstance) IsPhysical() bool {
	return d.HasCategory(Physical)
}

func (d DamageInstance) IsEnergy() bool {
	return d.HasCategory(Energy)
}

func (d DamageInstance) IsModifier() bool {
	return d.HasCategory(Modifier)
}
