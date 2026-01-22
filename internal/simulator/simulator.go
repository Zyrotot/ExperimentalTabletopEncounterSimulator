package simulator

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/arena"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/engine"
)

type Result struct {
	EncountersWon   int
	EnemiesDefeated int
	FinalRound      int
}

type Simulator interface {
	Run() Result
}

type EncounterBuilder interface {
	Build(encounterNumber int) *engine.Encounter
}

type LinearEncounterBuilder struct {
	PlayerFactory func() *combat.Combatant
	EnemyFactory  func() *combat.Combatant

	Arena arena.Arena
}

func (b *LinearEncounterBuilder) Build(n int) *engine.Encounter {
	enc := &engine.Encounter{}

	enc.Allies = []*combat.Combatant{
		b.PlayerFactory(),
	}

	for range n {
		enc.Enemies = append(enc.Enemies, b.EnemyFactory())
	}

	enc.Arena = b.Arena

	return enc
}

type EndlessSimulator struct {
	Engine  engine.Engine
	Builder EncounterBuilder
}

func anyAlive(chars []*combat.Combatant) bool {
	for _, c := range chars {
		if !c.Char.IsDead() {
			return true
		}
	}
	return false
}

func (s *EndlessSimulator) Run() Result {
	result := Result{}

	encounterNumber := 1

	for {
		log.Infof("Starting encounter %d", encounterNumber)
		enc := s.Builder.Build(encounterNumber)

		s.Engine.Run(enc)

		if !anyAlive(enc.Allies) {
			log.Infof("Player defeated in encounter %d", encounterNumber)
			break
		}

		log.Infof("Encounter %d completed", encounterNumber)
		result.EncountersWon++
		result.EnemiesDefeated += len(enc.Enemies)
		encounterNumber++
	}

	return result
}
