package main

import (
	"fmt"
	"math/rand"
	"os"
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

// Does the Character has Cleave? true or false  // TODO(rafael.moresco): enum para poder ter great cleave
var Cleave bool = false

// Is the Character immune to flanking? true or false
var FlankImmune bool = false

// Character attacks and damage, following the template "attack_name xdy+z #critrange #attack_bonus #crit_multyplier"
// where: x is the number of dice, y the dice type, z the damage modifier
// more than one value can be added
var attacks = []string{"SwordAttack1 5d6+34 19 20 2", "SwordAttack2 5d6+34 19 20 2"}

// Select enemy difficulty, ranging from 1 to 3
var difficulty int = 1

// Select arena type, where 1 is an open field, 2 is the Character in front of a wall, 3 is the Character in a corner between two walls
var arena int = 3

type Battlefield struct {
	ArenaType int
}

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

type PrintLog struct {
	level int
}

const (
	Uktril    int = 1
	Geraktril int = 2
	Reishid   int = 3
)

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
	fmt.Printf("%s attacks %s...\n", c.Name, target.Name)
	for _, attack := range c.Attacks {
		fmt.Println(attack)
		diceRoll := rollDice("1d20")
		attackRoll := diceRoll + attack.AttackBonus
		if attackRoll >= target.AC || diceRoll == 20 {
			damage := rollDice(attack.DamageDice)
			if !target.CritImmune && diceRoll >= attack.CritRange {
				damage = damage * attack.CritBonus
				fmt.Printf("(%d) Critical Hit! %s takes %d damage from %s.\n", diceRoll, target.Name, damage, attack.Name)
				target.takeDamage(damage)
			} else {
				fmt.Printf("(%d) Hit! %s takes %d damage from %s.\n", diceRoll, target.Name, damage, attack.Name)
				target.takeDamage(damage)
			}
		} else {
			fmt.Printf("%s misses (%d) %s with %s.\n", c.Name, diceRoll, target.Name, attack.Name)
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

func monsterFactory(monsterType int) *Character { // TODO(Zyrotot): Create logic for monsters bigger than medium
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
	case Reishid: // TODO(Zyrotot): Add alignment to characters, so that anti-alignment weapons can work
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
	var printer PrintLog

	if len(os.Args) > 0 {
		printer.level, _ = strconv.Atoi(os.Args[0])
	} else {
		printer.level = 0
	}

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
		fmt.Printf("Encounter %d begins!\n", encounter)
		player.CurrentHP = player.MaxHP
		enemies = nil

		for i := 0; i < encounter; i++ {
			fmt.Printf("Criando monstro %d\n", i)
			enemy := monsterFactory(difficulty)
			enemies = append(enemies, enemy)
			fmt.Println(enemies[i])
		}
		for {
			index := indexOfFirstAliveEnemy(enemies)
			if index == -1 {
				fmt.Printf("Encounter %d finished \n", encounter)
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
					fmt.Println("Enemy is defeated!")
				}
				if !player.isAlive() {
					break
				}
			}
			if !player.isAlive() {
				fmt.Println("Player has been defeated!")
				break
			}
		}

		fmt.Printf("Player HP: %d\n", player.CurrentHP)
		if player.isAlive() {
			fmt.Println("Proceeding to next encounter with more enemies...")
			encounter++
		} else {
			fmt.Printf("You managed to defeat %d enemies!", encounter-1)
			break
		}
	}
}
