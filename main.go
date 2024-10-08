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
var HP int = 205

// Character Armor Class
var AC int = 43

// Damage Resistance
var DR int = 0

// Fortification (chance to ignore critical hits). Must be placed as whole number, 25% = 25
// If immune, to critical hits, put 100
var Fort int = 100

// Does the creatura has Cura Acelerada? Whole number
var CuraAcelerada int = 0

// How many stacks of duro de matar does the character have? Whole number
var DuroDeMatar int = 0

// How many stacks of duro de ferir does the character have? Whole number
var DuroDeFerir int = 0

// Does the Character has Cleave? (0 = No, 1 = Cleave, 2 = Great Cleave)
var Cleave int = 0

// Is the Character immune to flanking? true or false
var FlankImmune bool = false

// Does the Character have Rigidez Raivosa? true or false
var RigidezRaivosa bool = true

// Does the character have Perfect Mobility? true or false
var PerfectMobility bool = false

// Does the weapon have Vampiric Weapon? true or false
var VampiricWeapon bool = false

// Does the character have Erosion? true or false
var Erosion bool = false

// Character attacks and damage, following the template "attack_name #attack_bonus xdy+z #critrange #crit_multyplier"
// where: x is the number of dice, y the dice type, z the damage modifier
// more than one value can be added
var attacks = []string{"SwordAttack1 21 5d6+36 19 2", "SwordAttack2 21 5d6+36 19 2"}

// Select enemy difficulty, ranging from 1 to 3
var difficulty int = 1

// Select arena type, where 1 is an open field, 2 is the Character in front of a wall, 3 is the Character in a corner between two walls
var arena int = 1

// Run mode: 0 = Result - Just the end results 1 = Info - The result for each encounter; 2 = Notice - Shows the fight; 3 = Debug - For debbuging purposes
var runmode int = 0

// Number of runs, to take mean
var runs int = 1000

// ---------------------------------   CODE  ---------------------------------------------------

type Attack struct {
	Name        string
	DamageDice  string
	AttackBonus int
	CritRange   int
	CritBonus   int
}

type TempBonus struct {
	AC int
	DR int
	HP int
}

type Character struct {
	Name            string
	CurrentHP       int
	MaxHP           int
	AC              int
	DR              int
	Fort            int
	CuraAcelerada   int
	DuroDeMatar     int
	DuroDeFerir     int
	Immortal        int
	Cleave          int
	FlankImmune     bool
	RigidezRaivosa  bool
	VampiricWeapon  bool
	Erosion         bool
	PerfectMobility bool
	IsFlanked       bool
	IsNPC           bool
	Attacks         []Attack
	TemporaryBonus  TempBonus
}

type Battlefield struct {
	ArenaType int
}

type Logger struct {
	level int
}

var logger Logger

const (
	BASE   int = 0
	INFO   int = 1
	NOTICE int = 2
	DEBUG  int = 3
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
		AttackBonus, _ := strconv.Atoi(split[1])
		CritRange, _ := strconv.Atoi(split[3])
		CritBonus, _ := strconv.Atoi(split[4])
		NewAttack := Attack{split[0], split[2], AttackBonus, CritRange, CritBonus}
		AttacksList = append(AttacksList, NewAttack)
	}
	return AttacksList
}

func (c *Character) takeDamage(damage int, erosion bool) {
	if damage < 1 {
		damage = 1
	}
	if erosion {
		if c.TemporaryBonus.DR > 0 {
			c.TemporaryBonus.DR --
		} else if c.DR > 0 {
			c.DR--
		}
	}

	if c.DR+c.TemporaryBonus.DR > 0 {
		damage -= c.DR + c.TemporaryBonus.DR
		if damage < 1 {
			damage = 0
		}
		if c.DuroDeFerir > 0 && damage > 0 {
			logger.Log(NOTICE, "%d damage negated due to Duro de ferir, remaining stacks %d.\n", damage, c.DuroDeFerir-1)
			damage = 0
			c.DuroDeFerir--
		} else {
			logger.Log(NOTICE, "%d damage taken due to %d DR.\n", damage, c.DR+c.TemporaryBonus.DR)
		}
	} else if c.DuroDeFerir > 0 && damage > 0 {
		logger.Log(NOTICE, "%d damage negated due to Duro de ferir, remaining stacks %d.\n", damage, c.DuroDeFerir-1)
		damage = 0
		c.DuroDeFerir--
	}

	if c.RigidezRaivosa && damage > 0 {
		c.TemporaryBonus.DR += 1
	}

	logger.Log(DEBUG, "Current DR %d.\n", c.DR+c.TemporaryBonus.DR)

	if c.CurrentHP-damage < 0 && c.DuroDeMatar > 0 {
		logger.Log(NOTICE, "Death avoided, %d damage negated due to Duro de matar, remaining stacks %d.\n", damage, c.DuroDeMatar-1)
		damage = 0
		c.DuroDeMatar--
	}

	if c.TemporaryBonus.HP > 0 && damage > 0 {
		if c.TemporaryBonus.HP < damage {
			damage = damage - c.TemporaryBonus.HP
			c.TemporaryBonus.HP = 0
		} else {
			c.TemporaryBonus.HP -= damage
			damage = 0
		}
	}

	c.CurrentHP -= damage
	logger.Log(DEBUG, "Current HP %d.\n", c.CurrentHP)
	if c.CurrentHP < 0 {
		c.CurrentHP = 0
		if c.IsNPC {
			logger.Log(NOTICE, "Enemy defeated\n")
		}
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
		if !target.isAlive() {
			break
		}
		logger.Log(DEBUG, "%v\n", attack)
		diceRoll := rollDice("1d20")
		attackRoll := diceRoll + attack.AttackBonus
		if target.IsFlanked {
			attackRoll = attackRoll + 2
		}
		logger.Log(DEBUG, "Total AC: %v\n", target.AC+target.TemporaryBonus.AC)
		if diceRoll != 1 && (attackRoll >= target.AC+target.TemporaryBonus.AC || diceRoll == 20) {
			damage := rollDice(attack.DamageDice)
			if diceRoll >= attack.CritRange {
				if target.Fort == 0 {
					damage = damage * attack.CritBonus
					logger.Log(NOTICE, "(%d) Critical Hit! %s deals %d damage to %s.\n", diceRoll, attack.Name, damage, target.Name)
				} else {
					if rand.Intn(100) > target.Fort {
						damage = damage * attack.CritBonus
						logger.Log(NOTICE, "(%d) Critical Hit! %s deals %d damage to %s.\n", diceRoll, attack.Name, damage, target.Name)
					} else {
						logger.Log(NOTICE, "(%d) Hit! %s deals %d damage to %s.\n", diceRoll, attack.Name, damage, target.Name)
					}
				}
			} else {
				logger.Log(NOTICE, "(%d) Hit! %s deals %d damage to %s.\n", diceRoll, attack.Name, damage, target.Name)
			}
			if c.VampiricWeapon {
				if damage/2 > c.TemporaryBonus.HP {
					c.TemporaryBonus.HP = damage/2
				}
			}
			target.takeDamage(damage, c.Erosion)
		} else {
			logger.Log(NOTICE, "(%d) %s misses %s with %s.\n", diceRoll, c.Name, target.Name, attack.Name)
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
			Name:      "Uktril",
			CurrentHP: 60,
			AC:        22,
			Fort:      100,
			Attacks: []Attack{
				{"Pinça", "1d8+8", 13, 20, 2},
				{"Garra", "1d4+8", 12, 20, 2},
			},
			// DR: 5,
			IsNPC: true,
		}
	case Geraktril:
		return &Character{
			Name:      "Geraktril",
			CurrentHP: 99,
			AC:        25,
			Fort:      100,
			Attacks: []Attack{
				{"Pinça", "1d8+10", 17, 20, 2},
				{"Pinça", "1d8+10", 17, 20, 2},
				{"Garra", "1d4+10", 16, 20, 2},
			},
			// DR: 10,
			IsNPC: true,
		}
	case Reishid:
		return &Character{
			Name:      "Reishid",
			CurrentHP: 143,
			AC:        30,
			Fort:      100,
			Attacks: []Attack{
				{"Adaga", "1d4+14", 26, 19, 2},
				{"Mordida", "1d4+10", 22, 20, 2},
				{"Garra", "1d4+10", 22, 20, 2},
			},
			// DR: 10,
			IsNPC: true,
		}
	default:
		return &Character{
			Name:      "Darius",
			CurrentHP: 130,
			AC:        27,
			Fort:      30,
			DR:		   13,
			CuraAcelerada: 5,
			Attacks: []Attack{
				{"Machandejante", "4d6+27", 20, 20, 3},
			},
			IsNPC: true,
		}
	}
}

func main() {
	logger = Logger{runmode}

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
		Name:            Name,
		MaxHP:           HP,
		AC:              AC,
		DR:              DR,
		Fort:            Fort,
		CuraAcelerada:   CuraAcelerada,
		DuroDeMatar:     DuroDeMatar,
		DuroDeFerir:     DuroDeFerir,
		Immortal:        0,
		Cleave:          Cleave,
		FlankImmune:     FlankImmune,
		RigidezRaivosa:  RigidezRaivosa,
		VampiricWeapon:	 VampiricWeapon,
		Erosion:		 Erosion,
		PerfectMobility: PerfectMobility,
		Attacks:         attackParser(),
	}
	var enemies []*Character

	run := 1
	defeated := 0
	for {
		encounter := 1
		for {
			logger.Log(INFO, "Encounter %d begins!\n", encounter)
			player.CurrentHP = player.MaxHP
			player.TemporaryBonus = TempBonus{0, 0, 0}
			player.DuroDeFerir = DuroDeFerir
			player.DuroDeMatar = DuroDeMatar
			enemies = nil
			ascended := false

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
				if player.Cleave == 2 && !enemies[index].isAlive() {
					for i := index+1; i < len(enemies); i++ {
						logger.Log(NOTICE, "Enemy cleaved \n")
						player.attack(enemies[i])
						if enemies[i].isAlive() || numberOfAliveEnemies(enemies) < 3 {
							break
						}
					}
				} else if player.Cleave == 1 && !enemies[index].isAlive() {
					if index+1 < len(enemies) && numberOfAliveEnemies(enemies) > 2 {
						logger.Log(NOTICE, "Enemy cleaved \n")
						player.attack(enemies[index+1])
					}
				}
				count := 0

				if player.CuraAcelerada > 0 && player.CurrentHP < player.MaxHP {
					logger.Log(DEBUG, "Healing %d, current HP %d, current DR %d\n", player.CuraAcelerada, player.CurrentHP, player.DR+player.TemporaryBonus.DR)
					player.CurrentHP += player.CuraAcelerada
					if player.RigidezRaivosa {
						player.TemporaryBonus.DR = 0
					}
					if player.CurrentHP > player.MaxHP {
						player.CurrentHP = player.MaxHP
					}
					logger.Log(DEBUG, "Current HP %d and DR %d after healing\n", player.CurrentHP, player.DR+player.TemporaryBonus.DR)
				}

				if numberOfAliveEnemies(enemies) > 2 && battlefield.ArenaType != 3 {
					if !player.FlankImmune {
						player.IsFlanked = true
					}
				}

				if numberOfAliveEnemies(enemies) < 4 && player.PerfectMobility {
					player.TemporaryBonus.AC = 2
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
			switch difficulty {
			case 1:
				if (player.DR+player.TemporaryBonus.DR >= 32) || (player.Fort == 100 && player.DR+player.TemporaryBonus.DR >= 16) {
					player.Immortal++
					ascended = true
				}
			case 2:
				if (player.DR+player.TemporaryBonus.DR >= 36) || (player.Fort == 100 && player.DR+player.TemporaryBonus.DR >= 18) {
					player.Immortal++
					ascended = true
				}
			case 3:
				if (player.DR+player.TemporaryBonus.DR >= 36) || (player.Fort == 100 && player.DR+player.TemporaryBonus.DR >= 18) {
					player.Immortal++
					ascended = true
				}
			default:
			}
			if ascended {
				logger.Log(INFO, "\nImmortality Achieved!")
				encounter++
			}
			logger.Log(INFO, "Player HP: %d\n", player.CurrentHP)
			if player.isAlive() {
				logger.Log(INFO, "%v\n", "Proceeding to next encounter with more enemies...")
				if !ascended {
					encounter++
				}
			} else {
				logger.Log(INFO, "You managed to defeat %d enemies!", encounter-1)
				defeated += encounter - 1
				break
			}
		}
		run++
		if run > runs {
			var mean_defeated float64 = float64(defeated) / float64(run-1)
			logger.Log(BASE, "\nYour mean number of defeated enemies was %f!", mean_defeated)
			if player.Immortal != 0 {
				logger.Log(BASE, "\nYou where immortal %d times!", player.Immortal)
			}
			break
		} else {
			logger.Log(BASE, "\nRun %d finished your number of defeated enemies was %d!", run-1, encounter-1)
		}
	}
}
