package main

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/monsters"
)

func main() {
	character := combat.Combatant{}
	attack := entity.Attack{
		Name:        "SwordAttack1",
		DamageDice:  "5d6+36",
		AttackBonus: 21,
		CritRange:   19,
		CritBonus:   2,
	}
	character.Char = &entity.Character{
		Name: "Bob",
		Stats: entity.Stats{
			MaxHP: 205,
			AC:    43,
			DR:    0,
			Fort:  100,
		},
		Runtime: entity.Runtime{
			HP: 205,
			AC: 43,
			DR: 0,
		},
		Attacks: []entity.Attack{
			attack,
			attack,
		},
		IsNPC: false,
	}

	base_enemie := combat.Combatant{}
	base_enemie.Char = monsters.MonsterFactory(monsters.Geraktril)

	combatLog := logging.New("combat", logging.INFO)
	diceLog := logging.New("dice", logging.WARN)

	resolver := combat.NewResolver(dice.NewRandomRoller(diceLog), combatLog)

	resolver.ResolveAttack(&character, &base_enemie)
}

// {
//   "curaAcelerada": 0,
//   "duroDeMatar": 0,
//   "duroDeFerir": 0,
//   "cleave": 0,
//   "flankImmune": false,
//   "rigidezRaivosa": true,
//   "perfectMobility": false,
//   "vampiricWeapon": false,
//   "erosion": false,
//   "attacks": [
//     {
//       "name": "SwordAttack1",
//       "attackBonus": 21,
//       "damageDice": "5d6+36",
//       "critRange": 19,
//       "critBonus": 2
//     },
//     {
//       "name": "SwordAttack2",
//       "attackBonus": 21,
//       "damageDice": "5d6+36",
//       "critRange": 19,
//       "critBonus": 2
//     }
//   ]
// }
