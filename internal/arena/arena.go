package arena

import "github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"

type Arena interface {
	MaxEngagers(target *combat.Combatant) int
	AllowsFlanking(engagers int) bool
}

type OpenField struct{}

func (OpenField) MaxEngagers(_ *combat.Combatant) int {
	return 8
}

func (OpenField) AllowsFlanking(engagers int) bool {
	return engagers >= 2
}

type AgainstWall struct{}

func (AgainstWall) MaxEngagers(_ *combat.Combatant) int {
	return 5
}

func (AgainstWall) AllowsFlanking(engagers int) bool {
	return engagers >= 2
}

type AgainstCorner struct{}

func (AgainstCorner) MaxEngagers(_ *combat.Combatant) int {
	return 3
}

func (AgainstCorner) AllowsFlanking(_ int) bool {
	return false
}

type EngagementTracker struct {
	arena  Arena
	active map[*combat.Combatant]map[*combat.Combatant]struct{}
}

func NewEngagementTracker(a Arena) *EngagementTracker {
	return &EngagementTracker{
		arena:  a,
		active: make(map[*combat.Combatant]map[*combat.Combatant]struct{}),
	}
}

func (e *EngagementTracker) CanEngage(attacker, target *combat.Combatant) bool {
	engagers := e.active[target]
	if engagers == nil {
		return true
	}
	return len(engagers) < e.arena.MaxEngagers(target)
}

func (e *EngagementTracker) Engage(attacker, target *combat.Combatant) {
	if e.active[target] == nil {
		e.active[target] = make(map[*combat.Combatant]struct{})
	}
	e.active[target][attacker] = struct{}{}
}

func (e *EngagementTracker) ResetTurn() {
	e.active = make(map[*combat.Combatant]map[*combat.Combatant]struct{})
}
