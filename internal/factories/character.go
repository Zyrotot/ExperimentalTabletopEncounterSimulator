package factories

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

func PlayerFactory() *combat.Combatant {
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
	}
	character.Attacks = []combat.Attack{attack, attack}

	return &character
}
