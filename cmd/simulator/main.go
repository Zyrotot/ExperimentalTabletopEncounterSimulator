package main

import (
	"fmt"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/engine"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/factories"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/simulator"
)

func main() {
	combatLog := logging.New("combat", logging.DEBUG)
	diceLog := logging.New("dice", logging.DEBUG)
	entityLog := logging.New("entity", logging.DEBUG)
	factoriesLog := logging.New("factories", logging.DEBUG)

	combat.SetLogger(combatLog)
	dice.SetLogger(diceLog)
	entity.SetLogger(entityLog)
	factories.SetLogger(factoriesLog)

	resolver := combat.NewResolver(dice.NewRandomRoller())
	engine := &engine.AutoEngine{Resolver: resolver}

	builder := &simulator.LinearEncounterBuilder{
		PlayerFactory: func() *combat.Combatant {
			return factories.PlayerFactory()
		},
		EnemyFactory: func() *combat.Combatant {
			return factories.MonsterFactory(factories.Uktril)
		},
	}

	sim := &simulator.EndlessSimulator{
		Engine:  engine,
		Builder: builder,
	}

	result := sim.Run()

	fmt.Printf(
		"Encounters won: %d\nEnemies defeated: %d\n",
		result.EncountersWon,
		result.EnemiesDefeated,
	)
}
