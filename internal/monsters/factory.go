package monsters

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/combat"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/dice"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/rules"
)

const (
	Uktril    int = 1
	Geraktril int = 2
	Reishid   int = 3
)

func MonsterFactory(monsterType int) *combat.Combatant {
	switch monsterType {
	case Uktril:
		return &combat.Combatant{
			Char: &entity.Character{
				Name: "Uktril",
				Stats: entity.Stats{
					MaxHP: 60,
					AC:    22,
					DR:    []entity.DamageReduction{{Value: 5, BypassType: rules.Magic}}, // DR 5/magic
					Fort:  100,
				},
				Runtime: entity.Runtime{
					HP: 60,
					AC: 22,
					DR: []entity.DamageReduction{{Value: 5, BypassType: rules.Magic}}, // DR 5/magic
				},
				Alignment: entity.Alignment{
					Moral: entity.Evil,
					Ethic: entity.Chaotic,
				},
				IsNPC: true,
			},
			Attacks: []combat.Attack{
				{
					Name: "Pinça",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Slash},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d8+8
										Count: 1,
										Sides: 8,
										Flat:  8,
									},
								},
							},
						},
					},
					AttackBonus: 13,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name: "Garra",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Slash},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d4+8
										Count: 1,
										Sides: 4,
										Flat:  8,
									},
								},
							},
						},
					},
					AttackBonus: 12,
					CritRange:   20,
					CritBonus:   2,
				},
			},
		}
	case Geraktril:
		return &combat.Combatant{
			Char: &entity.Character{
				Name: "Geraktril",
				Stats: entity.Stats{
					MaxHP: 99,
					AC:    25,
					DR:    []entity.DamageReduction{{Value: 10, BypassType: rules.Magic}}, // DR 10/magic
					Fort:  100,
				},
				Runtime: entity.Runtime{
					HP: 99,
					AC: 25,
					DR: []entity.DamageReduction{{Value: 10, BypassType: rules.Magic}}, // DR 10/magic
				},
				Alignment: entity.Alignment{
					Moral: entity.Evil,
					Ethic: entity.Chaotic,
				},
				IsNPC: true,
			},
			Attacks: []combat.Attack{
				{
					Name: "Pinça",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Slash},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d8+10
										Count: 1,
										Sides: 8,
										Flat:  10,
									},
								},
							},
						},
					},
					AttackBonus: 17,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name: "Garra",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Slash},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d4+10
										Count: 1,
										Sides: 4,
										Flat:  10,
									},
								},
							},
						},
					},
					AttackBonus: 16, CritRange: 20,
					CritBonus: 2,
				},
			},
		}
	case Reishid:
		return &combat.Combatant{
			Char: &entity.Character{
				Name: "Reishid",
				Stats: entity.Stats{
					MaxHP: 143,
					AC:    30,
					DR:    []entity.DamageReduction{{Value: 10, BypassType: rules.Magic}}, // DR 10/magic
					Fort:  100,
				},
				Runtime: entity.Runtime{
					HP: 143,
					AC: 30,
					DR: []entity.DamageReduction{{Value: 10, BypassType: rules.Magic}}, // DR 10/magic
				},
				Alignment: entity.Alignment{
					Moral: entity.Evil,
					Ethic: entity.Chaotic,
				},
				IsNPC: true,
			},
			Attacks: []combat.Attack{
				{
					Name: "Adaga",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Pierce, rules.Magic},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d4+14
										Count: 1,
										Sides: 4,
										Flat:  14,
									},
								},
							},
						},
					},
					DamageModifiers: []combat.DamageModifier{
						combat.EvilWeapon{
							BaseDamageModifier: combat.BaseDamageModifier{
								Term: dice.Term{
									Count: 2,
									Sides: 6,
								},
							},
						},
					},
					AttackBonus: 26,
					CritRange:   19,
					CritBonus:   2,
				},
				{
					Name: "Mordida",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Pierce},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d4+14
										Count: 1,
										Sides: 4,
										Flat:  14,
									},
								},
							},
						},
					},
					AttackBonus: 22,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name: "Garra",
					DamageDice: []combat.DamageExpression{
						{
							DamageTypes: []rules.DamageType{rules.Slash},
							DamageRoll: dice.Expression{
								Terms: []dice.Term{
									{ // 1d4+10
										Count: 1,
										Sides: 4,
										Flat:  10,
									},
								},
							},
						},
					},
					AttackBonus: 22,
					CritRange:   20,
					CritBonus:   2,
				},
			},
		}
	default:
		// custom_enemy, err := LoadCharacterFromJSON("custom_enemy.json")
		// if err == nil {
		// 	return custom_enemy
		// }
		return MonsterFactory(1)
	}
}

// func LoadCharacterFromJSON(filename string) (*entity.Character, error) {
// 	data, err := os.ReadFile(filename)
// 	if err != nil {
// 		return nil, err
// 	}

// 	var c entity.Character
// 	if err := json.Unmarshal(data, &c); err != nil {
// 		return nil, err
// 	}

// 	// Initialize runtime values
// 	c.CurrentHP = c.MaxHP
// 	c.Immortal = 0
// 	c.IsNPC = false
// 	c.TemporaryBonus = TempBonus{0, 0, 0}

// 	return &c, nil
// }
