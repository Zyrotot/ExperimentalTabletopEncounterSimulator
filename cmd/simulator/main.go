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
	attack := combat.Attack{
		Name: "SwordAttack1",
		DamageDice: combat.DamageExpression{
			Components: map[combat.DamageType]dice.Term{
				combat.Pierce: { // 5d6+36
					Count: 5,
					Sides: 6,
					Flat:  36,
				},
			},
		},
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
		IsNPC: false,
	}
	character.Attacks = []combat.Attack{attack, attack}

	base_enemie := monsters.MonsterFactory(monsters.Geraktril)

	combatLog := logging.New("combat", logging.DEBUG)
	diceLog := logging.New("dice", logging.DEBUG)

	resolver := combat.NewResolver(dice.NewRandomRoller(diceLog), combatLog)

	resolver.ResolveAttack(&character, base_enemie)
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
