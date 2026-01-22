package entity

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"

type Runtime struct {
	HP     int
	TempHP int

	ACBonus int

	AddedImmunities []Immunity
	AddedWeaknesses []Weakness
	AddedDR         []DamageReduction
	AddedER         []EnergyResistance

	DRSuppressed []DRSuppression

	EngagedCount int
	IsFlanked    bool
}

func (rt *Runtime) GetTotalHP() int {
	return rt.HP + rt.TempHP
}

type DamageFilter struct {
	Type     rules.DamageType
	Category rules.DamageCategory
}

func (f DamageFilter) Matches(d rules.DamageInstance) bool {
	if f.Type != "" && d.HasType(f.Type) {
		return true
	}
	if f.Category != 0 && d.HasCategory(f.Category) {
		return true
	}
	return false
}

func (wk Weakness) Priority() int { return 10 }

func (wk Weakness) Apply(damage []rules.DamageInstance) {
	for i := range damage {
		if wk.Filter.Matches(damage[i]) {
			log.Infof("Weakness of %d applied", wk.Value)
			damage[i].Amount += wk.Value
			break
		}
	}
}

func (im Immunity) Priority() int { return 20 }

func (im Immunity) Apply(damage []rules.DamageInstance) {
	for i := range damage {
		if im.Filter.Matches(damage[i]) {
			log.Infof("Immunity applied")
			damage[i].Amount = 0
			break
		}
	}
}

func (er EnergyResistance) Priority() int { return 30 }

func (er EnergyResistance) Apply(damage []rules.DamageInstance) {
	remaining := er.Value

	for i := range damage {
		if remaining <= 0 {
			break
		}

		if !er.Filter.Matches(damage[i]) {
			continue
		}

		reduced := min(damage[i].Amount, remaining)
		log.Infof("ER - Reduced by %d", reduced)

		damage[i].Amount -= reduced
		remaining -= reduced
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

func (dr DamageReduction) Priority() int { return 40 }

func (dr DamageReduction) Apply(damage []rules.DamageInstance) {
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
