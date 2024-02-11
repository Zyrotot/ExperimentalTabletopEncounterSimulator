package main

import (
	"fmt"
	"math/rand"
	"os"
	"regexp"
	"strconv"
)

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
			fmt.Printf("%s misses %s with %s.\n", c.Name, target.Name, attack.Name)
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

func main() {
	var printer PrintLog

	if len(os.Args) > 0 {
		printer.level, _ = strconv.Atoi(os.Args[0])
	} else {
		printer.level = 0
	}

	player := Character{
		Name:        "Paladino Sedutor Meio-Dragão",
		MaxHP:       195,
		AC:          42,
		CritImmune:  true,
		Cleave:      false,
		FlankImmune: false,
		Attacks: []Attack{
			{"Espada grande", "5d6+34", 19, 25, 2},
		},
	}
	var enemies []*Character

	encounter := 1
	for {
		fmt.Printf("Encounter %d begins!\n", encounter)
		player.CurrentHP = player.MaxHP
		enemies = nil

		for i := 0; i < encounter; i++ {
			fmt.Printf("Criando monstro %d\n", i)
			enemy := &Character{
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

			if numberOfAliveEnemies(enemies) > 2 {
				if !player.FlankImmune {
					player.IsFlanked = true
				}
			}

			for i, enemy := range enemies {
				if enemy.isAlive() {
					count++
					if count <= 8 {
						if numberOfAliveEnemies(enemies)%2 == 1 && len(enemies)-1 == i {
							player.IsFlanked = false
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
