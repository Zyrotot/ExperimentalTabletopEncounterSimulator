package main

import (
	"fmt"
	"math/rand"
	"regexp"
	"strconv"
	"time"
)

type Attack struct {
	Name        string
	DamageDice  string
	AttackBonus int
	CritBonus   int
}

type Character struct {
	Name       string
	HP         int
	AC         int
	CritImmune bool
	Attacks    []Attack
}

func (c *Character) takeDamage(damage int) {
	c.HP -= damage
	if c.HP < 0 {
		c.HP = 0
	}
}

func (c *Character) isAlive() bool {
	return c.HP > 0
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
			if !target.CritImmune && diceRoll == 20 {
				damage = damage * attack.CritBonus
				fmt.Printf("Critical Hit! %s takes %d damage from %s.\n", target.Name, damage, attack.Name)
				target.takeDamage(damage)
			} else {
				if diceRoll == 20 {
					fmt.Printf("Critical Hit! %s takes %d damage from %s.\n", target.Name, damage, attack.Name)
				} else {
					fmt.Printf("Hit! %s takes %d damage from %s.\n", target.Name, damage, attack.Name)
				}
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

func main() {
	rand.Seed(time.Now().UnixNano()) // Trocar aqui para mudar o resultado

	HPtotal := 195

	player := Character{
		Name:       "Player",
		HP:         HPtotal,
		AC:         42,
		CritImmune: true,
		Attacks: []Attack{
			{"Sword", "3d6+34", 25, 2},
		},
	}

	enemies := []*Character{}

	encounter := 1
	for player.isAlive() {
		fmt.Printf("Encounter %d begins!\n", encounter)
		player.HP = HPtotal

		enemies = nil

		for i := 0; i < encounter; i++ {
			fmt.Printf("Criando monsntro %d\n", i)
			enemy := &Character{
				Name:       "Enemy",
				HP:         99,
				AC:         25,
				CritImmune: true,
				Attacks: []Attack{
					{"Pinça", "1d8+10", 17, 2},
					{"Garra", "1d4+10", 16, 2},
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
			count := 0
			for _, enemy := range enemies {
				count++
				if !enemy.isAlive() {
					fmt.Printf("%s defeated!\n", enemy.Name)
				} else if count <= 8 {
					enemy.attack(&player)
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

		fmt.Printf("Player HP: %d\n", player.HP)
		if player.isAlive() {
			fmt.Println("Proceeding to next encounter with more enemies...")
			encounter++
		} else {
			fmt.Printf("You managed to defeat %d enemies!", encounter-1)
			break
		}
	}
}
