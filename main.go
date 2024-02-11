package main

import (
	"fmt"
	"math/rand"
	"regexp"
	"strconv"
	"strings"
)

///////////////////////////////////////////////
// EXPERIMENTAL TABLETOP ENCOUNTER SIMULATOR //
///////////////////////////////////////////////

// INPUTS:

// Character Name
var Name string = "Bob"

// Character HP
var HP int = 195

// Character Armor Class
var AC int = 42

// Is the Character immune to crits? true or false
var CritImmune bool = true

// Does the Character has Cleave? true or false
var Cleave bool = false

// Is the Character immune to flanking? true or false
var FlankImmune bool = false

// Character attacks and damage, following the template "attack_name xdy+z #critrange #attack_bonus #crit_multyplier"
// where: x is the number of dice, y the dice type, z the damage modifier
// more than one value can be added
var attacks = []string{"SwordAttack1 5d6+34 19 20 2", "SwordAttack2 5d6+34 19 20 2"}

// Select enemy difficulty, ranging from 1 to 3
var difficulty int = 2

// Select arena type, where 1 is an open field, 2 is the Character in front of a wall, 3 is the Character in a corner between two walls
var arena int = 1

// ---------------------------------   CODE  ---------------------------------------------------

type Attack struct {
	Name        string
	DamageDice  string
	AttackBonus int
	CritRange   int
	CritBonus   int
}

type Character struct {
	Name        string
	CurrentHP   int
	MaxHP       int
	AC          int
	CritImmune  bool
	Cleave      bool
	FlankImmune bool
	IsFlanked   bool
	Attacks     []Attack
}

type Battlefield struct {
	ArenaType int
}

type Logger struct {
	level int
}

var logger Logger

const (
	INFO   int = 5
	NOTICE int = 6
	DEBUG  int = 7
)

const (
	Uktril    int = 1
	Geraktril int = 2
	Reishid   int = 3
)

func (log *Logger) Log(level int, format string, a ...any) {
	if level <= log.level {
		fmt.Printf(format, a...)
	}
}

func attackParser() []Attack {
	AttacksList := []Attack{}
	for _, attack := range attacks {
		split := strings.Split(attack, " ")
		AttackBonus, _ := strconv.Atoi(split[2])
		CritRange, _ := strconv.Atoi(split[3])
		CritBonus, _ := strconv.Atoi(split[4])
		NewAttack := Attack{split[0], split[1], AttackBonus, CritRange, CritBonus}
		AttacksList = append(AttacksList, NewAttack)
	}
	return AttacksList
}

func (c *Character) takeDamage(damage int) {
	if damage < 1 {
		damage = 1
	}
	c.CurrentHP -= damage
	if c.CurrentHP < 0 {
		c.CurrentHP = 0
	}
}

func (c *Character) isAlive() bool {
	return c.CurrentHP > 0
}

func rollDice(dice string) int {
	re := regexp.MustCompile(`(\d+)d(\d+)([+-]\d+)?`)
	match := re.FindStringSubmatch(dice)
	if match == nil {
		return 0
	}
	numDice, _ := strconv.Atoi(match[1])
	diceSides, _ := strconv.Atoi(match[2])
	modifier := 0
	if len(match) > 3 {
		modifier, _ = strconv.Atoi(match[3])
	}

	total := modifier
	for i := 0; i < numDice; i++ {
		total += rand.Intn(diceSides) + 1
	}
	return total
}

func (c *Character) attack(target *Character) {
	logger.Log(NOTICE, "%s attacks %s...\n", c.Name, target.Name)
	for _, attack := range c.Attacks {
		logger.Log(DEBUG, "%v\n", attack)
		diceRoll := rollDice("1d20")
		attackRoll := diceRoll + attack.AttackBonus
		if diceRoll != 1 && (attackRoll >= target.AC || diceRoll == 20) {
			damage := rollDice(attack.DamageDice)
			if !target.CritImmune && diceRoll >= attack.CritRange {
				damage = damage * attack.CritBonus
				logger.Log(DEBUG, "(%d) Critical Hit! %s takes %d damage from %s.\n", diceRoll, target.Name, damage, attack.Name)
				target.takeDamage(damage)
			} else {
				logger.Log(DEBUG, "(%d) Hit! %s takes %d damage from %s.\n", diceRoll, target.Name, damage, attack.Name)
				target.takeDamage(damage)
			}
		} else {
			logger.Log(DEBUG, "%s misses (%d) %s with %s.\n", c.Name, diceRoll, target.Name, attack.Name)
		}
	}
}

func indexOfFirstAliveEnemy(enemies []*Character) int {
	for i, enemy := range enemies {
		if enemy.isAlive() {
			return i
		}
	}
	return -1
}

func numberOfAliveEnemies(enemies []*Character) int {
	count := 0
	for _, enemy := range enemies {
		if enemy.isAlive() {
			count++
		}
	}
	return count
}

func monsterFactory(monsterType int) *Character {
	switch monsterType {
	case Uktril:
		return &Character{
			Name:       "Uktril",
			CurrentHP:  60,
			AC:         22,
			CritImmune: true,
			Attacks: []Attack{
				{"Pinça", "1d8+8", 20, 13, 2},
				{"Garra", "1d4+8", 20, 12, 2},
			},
		}
	case Geraktril:
		return &Character{
			Name:       "Geraktril",
			CurrentHP:  99,
			AC:         25,
			CritImmune: true,
			Attacks: []Attack{
				{"Pinça", "1d8+10", 20, 17, 2},
				{"Pinça", "1d8+10", 20, 17, 2},
				{"Garra", "1d4+10", 20, 16, 2},
			},
		}
	case Reishid:
		return &Character{
			Name:       "Reishid",
			CurrentHP:  143,
			AC:         30,
			CritImmune: true,
			Attacks: []Attack{
				{"Adaga", "1d4+14", 19, 26, 2},
				{"Mordida", "1d4+10", 20, 22, 2},
				{"Garra", "1d4+10", 20, 22, 2},
			},
		}
	default:
		return &Character{
			Name:       "Uktril",
			CurrentHP:  60,
			AC:         22,
			CritImmune: true,
			Attacks: []Attack{
				{"Pinça", "1d8+8", 20, 13, 2},
				{"Garra", "1d4+8", 20, 12, 2},
			},
		}
	}
}

func main() {
	logger = Logger{INFO}

	battlefield := Battlefield{arena}

	var countLimit int

	switch battlefield.ArenaType {
	case 1:
		countLimit = 8
	case 2:
		countLimit = 5
	case 3:
		countLimit = 3
	default:
		countLimit = 8
	}

	player := Character{
		Name:        Name,
		MaxHP:       HP,
		AC:          AC,
		CritImmune:  CritImmune,
		Cleave:      Cleave,
		FlankImmune: FlankImmune,
		Attacks:     attackParser(),
	}
	var enemies []*Character

	encounter := 1
	for {
		logger.Log(INFO, "Encounter %d begins!\n", encounter)
		player.CurrentHP = player.MaxHP
		enemies = nil

		for i := 0; i < encounter; i++ {
			logger.Log(DEBUG, "Criando monstro %d\n", i)
			enemy := monsterFactory(difficulty)
			enemies = append(enemies, enemy)
			logger.Log(DEBUG, "%v\n", enemies[i])
		}
		for {
			index := indexOfFirstAliveEnemy(enemies)
			if index == -1 {
				logger.Log(INFO, "Encounter %d finished \n", encounter)
				break
			}
			player.attack(enemies[index])
			if player.Cleave && !enemies[index].isAlive() {
				if index+1 < len(enemies) && numberOfAliveEnemies(enemies) > 4 {
					player.attack(enemies[index+1])
				}
			}
			count := 0

			if numberOfAliveEnemies(enemies) > 2 && battlefield.ArenaType != 3 {
				if !player.FlankImmune {
					player.IsFlanked = true
				}
			}

			for i, enemy := range enemies {
				if enemy.isAlive() {
					count++
					if count <= countLimit {
						if battlefield.ArenaType == 1 {
							if numberOfAliveEnemies(enemies)%2 == 1 && len(enemies)-1 == i && numberOfAliveEnemies(enemies) < 8 {
								player.IsFlanked = false
							}
						} else if battlefield.ArenaType == 2 {
							if numberOfAliveEnemies(enemies) < 2 || count > 2 {
								player.IsFlanked = false
							}
						}

						enemy.attack(&player)
					}
				} else {
					logger.Log(DEBUG, "Enemy is defeated! \n")
				}
				if !player.isAlive() {
					break
				}
			}
			if !player.isAlive() {
				logger.Log(INFO, "%v\n", "Player has been defeated!")
				break
			}
		}

		logger.Log(INFO, "Player HP: %d\n", player.CurrentHP)
		if player.isAlive() {
			logger.Log(INFO, "%v\n", "Proceeding to next encounter with more enemies...")
			encounter++
		} else {
			logger.Log(INFO, "You managed to defeat %d enemies!", encounter-1)
			break
		}
	}
}
