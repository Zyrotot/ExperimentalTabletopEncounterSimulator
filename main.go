package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"regexp"
	"strconv"
)

///////////////////////////////////////////////
// EXPERIMENTAL TABLETOP ENCOUNTER SIMULATOR //
///////////////////////////////////////////////

// ---------------------------------   CODE  ---------------------------------------------------

type Attack struct {
	Name        string `json:"name"`
	DamageDice  string `json:"damageDice"`
	AttackBonus int    `json:"attackBonus"`
	CritRange   int    `json:"critRange"`
	CritBonus   int    `json:"critBonus"`
}

type TempBonus struct {
	AC int
	DR int
	HP int
}

type Character struct {
	Name             string    `json:"name"`
	CurrentHP        int       `json:"-"`
	MaxHP            int       `json:"hp"`
	AC               int       `json:"ac"`
	DR               int       `json:"dr"`
	Fort             int       `json:"fort"`
	CuraAcelerada    int       `json:"curaAcelerada"`
	DuroDeMatar      int       `json:"duroDeMatar"`
	DuroDeMatarAtual int       `json:"-"`
	DuroDeFerir      int       `json:"duroDeFerir"`
	DuroDeFerirAtual int       `json:"-"`
	Immortal         int       `json:"-"`
	Cleave           int       `json:"cleave"`
	FlankImmune      bool      `json:"flankImmune"`
	RigidezRaivosa   bool      `json:"rigidezRaivosa"`
	VampiricWeapon   bool      `json:"vampiricWeapon"`
	Erosion          bool      `json:"erosion"`
	PerfectMobility  bool      `json:"perfectMobility"`
	IsFlanked        bool      `json:"-"`
	IsNPC            bool      `json:"isNPC"`
	Attacks          []Attack  `json:"attacks"`
	TemporaryBonus   TempBonus `json:"-"`
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

func (c *Character) takeDamage(damage int, erosion bool) {
	if damage < 1 {
		damage = 1
	}
	if erosion {
		if c.TemporaryBonus.DR > 0 {
			c.TemporaryBonus.DR--
		} else if c.DR > 0 {
			c.DR--
		}
	}

	if c.DR+c.TemporaryBonus.DR > 0 {
		damage -= c.DR + c.TemporaryBonus.DR
		if damage < 1 {
			damage = 0
		}
		if c.DuroDeFerirAtual > 0 && damage > 0 {
			logger.Log(NOTICE, "%d damage negated due to Duro de ferir, remaining stacks %d.\n", damage, c.DuroDeFerirAtual-1)
			damage = 0
			c.DuroDeFerirAtual--
		} else {
			logger.Log(NOTICE, "%d damage taken due to %d DR.\n", damage, c.DR+c.TemporaryBonus.DR)
		}
	} else if c.DuroDeFerirAtual > 0 && damage > 0 {
		logger.Log(NOTICE, "%d damage negated due to Duro de ferir, remaining stacks %d.\n", damage, c.DuroDeFerirAtual-1)
		damage = 0
		c.DuroDeFerirAtual--
	}

	if c.RigidezRaivosa && damage > 0 {
		c.TemporaryBonus.DR += 1
	}

	logger.Log(DEBUG, "Current DR %d.\n", c.DR+c.TemporaryBonus.DR)

	if c.CurrentHP-damage < 0 && c.DuroDeMatarAtual > 0 {
		logger.Log(NOTICE, "Death avoided, %d damage negated due to Duro de matar, remaining stacks %d.\n", damage, c.DuroDeMatarAtual-1)
		damage = 0
		c.DuroDeMatarAtual--
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
					c.TemporaryBonus.HP = damage / 2
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
			DR:    5,
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
			DR:    10,
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
			DR:    10,
			IsNPC: true,
		}
	default:
		custom_enemy, err := LoadCharacterFromJSON("custom_enemy.json")
		if err == nil {
			return custom_enemy
		}
		return &Character{
			Name:      "Uktril",
			CurrentHP: 60,
			AC:        22,
			Fort:      100,
			Attacks: []Attack{
				{"Pinça", "1d8+8", 13, 20, 2},
				{"Garra", "1d4+8", 12, 20, 2},
			},
			DR:    5,
			IsNPC: true,
		}
	}
}

func LoadCharacterFromJSON(filename string) (*Character, error) {
	data, err := os.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	var c Character
	if err := json.Unmarshal(data, &c); err != nil {
		return nil, err
	}

	// Initialize runtime values
	c.CurrentHP = c.MaxHP
	c.Immortal = 0
	c.IsNPC = false
	c.TemporaryBonus = TempBonus{0, 0, 0}

	return &c, nil
}

func main() {
	difficulty := flag.Int("difficulty", 1, "Enemy difficulty (1 to 3)")
	arena := flag.Int("arena", 1, "Arena type (1=open, 2=wall, 3=corner)")
	runmode := flag.Int("runmode", 1, "Run mode (0=Result, 1=Info, 2=Notice, 3=Debug)")
	runs := flag.Int("runs", 100, "Number of runs to average")
	json := flag.String("json", "player.json", "Player character JSON file")
	flag.Parse()

	if *difficulty == -1 || *arena == -1 || *runmode == -1 || *runs == -1 {
		fmt.Println("Error: --difficulty, --arena, --runmode, and --runs are required")
		flag.Usage()
		os.Exit(1)
	}

	logger = Logger{*runmode}

	battlefield := Battlefield{*arena}

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

	player, err := LoadCharacterFromJSON(*json)
	if err != nil {
		panic(err)
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
			player.DuroDeFerirAtual = player.DuroDeFerir
			player.DuroDeMatarAtual = player.DuroDeMatar
			enemies = nil
			ascended := false

			for i := 0; i < encounter; i++ {
				logger.Log(DEBUG, "Criando monstro %d\n", i)
				enemy := monsterFactory(*difficulty)
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
					for i := index + 1; i < len(enemies); i++ {
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

							enemy.attack(player)
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
			switch *difficulty {
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
		if run > *runs {
			var mean_defeated float64 = float64(defeated) / float64(run-1)
			logger.Log(BASE, "\nYour mean number of defeated enemies was %f!\n", mean_defeated)
			if player.Immortal != 0 {
				logger.Log(BASE, "\nYou where immortal %d times!", player.Immortal)
			}
			break
		} else {
			logger.Log(BASE, "\nRun %d finished your number of defeated enemies was %d!", run-1, encounter-1)
		}
	}
}
