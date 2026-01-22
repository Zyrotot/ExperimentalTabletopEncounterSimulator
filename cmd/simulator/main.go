package main

import (
	"fmt"

	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/arena"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/engine"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/factories"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/simulator"
)

func main() {
	combatLog := logging.New("combat", logging.INFO)
	diceLog := logging.New("dice", logging.DEBUG)
	entityLog := logging.New("entity", logging.INFO)
	factoriesLog := logging.New("factories", logging.DEBUG)
	engineLog := logging.New("engine", logging.INFO)
	simulatorLog := logging.New("simulator", logging.INFO)

	combat.SetLogger(combatLog)
	dice.SetLogger(diceLog)
	entity.SetLogger(entityLog)
	factories.SetLogger(factoriesLog)
	engine.SetLogger(engineLog)
	simulator.SetLogger(simulatorLog)

	resolver := combat.NewResolver(dice.NewRandomRoller())
	targetSelector := &engine.FirstAliveSelector{}
	engine := &engine.AutoEngine{Resolver: resolver, TargetSelector: targetSelector}

	builder := &simulator.LinearEncounterBuilder{
		PlayerFactory: func() *combat.Combatant {
			return factories.PlayerFactory()
		},
		EnemyFactory: func() *combat.Combatant {
			return factories.MonsterFactory(factories.Uktril)
		},
		Arena: arena.OpenField{},
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
