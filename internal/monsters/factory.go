package monsters

import (
	"github.com/Zyrotot/ExperimentalTabletopEncounterSimulator/internal/entity"
)

const (
	Uktril    int = 1
	Geraktril int = 2
	Reishid   int = 3
)

func MonsterFactory(monsterType int) *entity.Character {
	switch monsterType {
	case Uktril:
		return &entity.Character{
			Name: "Uktril",
			Stats: entity.Stats{
				MaxHP: 60,
				AC:    22,
				DR:    5,
				Fort:  100,
			},
			Runtime: entity.Runtime{
				HP: 60,
				AC: 22,
				DR: 5,
			},
			Attacks: []entity.Attack{
				{
					Name:        "Pinça",
					DamageDice:  "1d8+8",
					AttackBonus: 13,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name:        "Garra",
					DamageDice:  "1d4+8",
					AttackBonus: 12,
					CritRange:   20,
					CritBonus:   2,
				},
			},
			IsNPC: true,
		}
	case Geraktril:
		return &entity.Character{
			Name: "Geraktril",
			Stats: entity.Stats{
				MaxHP: 99,
				AC:    25,
				DR:    10,
				Fort:  100,
			},
			Runtime: entity.Runtime{
				HP: 99,
				AC: 25,
				DR: 10,
			},
			Attacks: []entity.Attack{
				{
					Name:        "Pinça",
					DamageDice:  "1d8+10",
					AttackBonus: 17,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name:        "Pinça",
					DamageDice:  "1d8+10",
					AttackBonus: 17, CritRange: 20,
					CritBonus: 2,
				},
				{
					Name:        "Garra",
					DamageDice:  "1d4+10",
					AttackBonus: 16, CritRange: 20,
					CritBonus: 2,
				},
			},
			IsNPC: true,
		}
	case Reishid:
		return &entity.Character{
			Name: "Reishid",
			Stats: entity.Stats{
				MaxHP: 143,
				AC:    30,
				DR:    10,
				Fort:  100,
			},
			Runtime: entity.Runtime{
				HP: 143,
				AC: 30,
				DR: 10,
			},
			Attacks: []entity.Attack{
				{
					Name:        "Adaga",
					DamageDice:  "1d4+14",
					AttackBonus: 26,
					CritRange:   19,
					CritBonus:   2,
				},
				{
					Name:        "Mordida",
					DamageDice:  "1d4+10",
					AttackBonus: 22,
					CritRange:   20,
					CritBonus:   2,
				},
				{
					Name:        "Garra",
					DamageDice:  "1d4+10",
					AttackBonus: 22,
					CritRange:   20,
					CritBonus:   2,
				},
			},
			IsNPC: true,
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
