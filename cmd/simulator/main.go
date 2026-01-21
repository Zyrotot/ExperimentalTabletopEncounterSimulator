package main

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/logging"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/monsters"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

func main() {
	character := combat.Combatant{}
	attack := combat.Attack{
		Name: "SwordAttack1",
		DamageDice: []combat.DamageExpression{
			{
				DamageTypes: []rules.DamageType{rules.Slash, rules.Magic},
				DamageRoll: dice.Expression{
					Terms: []dice.Term{
						{ // 5d6+36
							Count: 5,
							Sides: 6,
							Flat:  36,
						},
					},
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
			Fort:  100,
		},
		Alignment: entity.Alignment{
			Moral: entity.Good,
			Ethic: entity.Lawful,
		},
		IsNPC: false,
	}
	character.Attacks = []combat.Attack{attack, attack}

	base_enemie := monsters.MonsterFactory(monsters.Geraktril)

	combatLog := logging.New("combat", logging.DEBUG)
	diceLog := logging.New("dice", logging.DEBUG)
	entityLog := logging.New("entity", logging.DEBUG)
	monstersLog := logging.New("monsters", logging.DEBUG)

	combat.SetLogger(combatLog)
	dice.SetLogger(diceLog)
	entity.SetLogger(entityLog)
	monsters.SetLogger(monstersLog)

	resolver := combat.NewResolver(dice.NewRandomRoller())

	resolver.ResolveAttack(&character, base_enemie)
	resolver.ResolveAttack(base_enemie, &character)
}
