// -----------------------------------------------------------------------------
// | @file      attack_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/attack_resolver.h"
#include "internal/logging/log_manager.h"

namespace internal {
namespace resolver {

using AttackResult = AttackResolver::AttackResult;

AttackResolver::AttackResolver(std::shared_ptr<Entity> attacker,
                               std::shared_ptr<Entity> defender,
                               const int& attack_sequence_index,
                               std::shared_ptr<Roller> roller)
    : attacker_(attacker),
      defender_(defender),
      attack_sequence_(attacker->GetAttackSequence(attack_sequence_index)),
      roller_(roller),
      logger_(logging::LogManager::GetLogger("attack")) {
}

AttackResolver::~AttackResolver() {
}

std::vector<AttackResult> AttackResolver::ResolveAttack() {
  std::vector<AttackResult> results;
  for (const auto& attack_move : attack_sequence_.attacks) {
    results.push_back(ResolveAttackMove(attack_move));
  }
  return results;
}

AttackResult AttackResolver::ResolveAttackMove(const AttackMove& attack_move) {
  AttackResult result;

  int attack_roll = roller_->Roll(Term{.dice_groups = {{1, 20}}});

  result.d20_roll = attack_roll;
  logger_->Debug("D20 roll: {}", attack_roll);

  attack_roll += CalculateTotalAttackModifier(attack_move);
  result.total_roll = attack_roll;

  int defender_ac = defender_->GetEffectiveAC();

  logger_->Info("{} rolls {} vs AC {}", attacker_->GetName(), attack_roll, defender_ac);

  if (attack_roll >= defender_ac) {
    result.hit = true;

    int crit_multiplier = CheckCriticalHit(attack_move, result.d20_roll);
    result.is_critical = crit_multiplier > 1;
    logger_->Debug("Critical multiplier: {}", crit_multiplier);

    Term damage_term = CalculateTotalDamage(attack_move);
    int damage = roller_->Roll(damage_term) * crit_multiplier;
    if (damage < 0) {
      damage = 1;
    }
    logger_->Info("Hit! Damage: {}", damage);
    result.damage = damage;
  }

  return result;
}

Term AttackResolver::CalculateTotalDamage(const AttackMove& attack_move) {
  Term total_damage = attack_move.GetAttackDamage();
  total_damage.AddModifier(attack_sequence_.damage_modifier);
  total_damage.AddModifier(
      attacker_->GetCurrentStats().base_stats.attack_bonuses.damage_bonus);

  return total_damage;
}

int AttackResolver::CalculateTotalAttackModifier(
    const AttackMove& attack_move) {
  int total_attack_modifier = attack_move.GetAttackModifier();
  total_attack_modifier += attack_sequence_.attack_modifier;
  total_attack_modifier +=
      attacker_->GetCurrentStats().base_stats.attack_bonuses.attack_bonus;

  return total_attack_modifier;
}

int AttackResolver::CheckCriticalHit(const AttackMove& attack_move,
                                     const int& attack_roll) {
  if (!attack_move.weapon) {
    return 1;
  }

  int crit_threshold =
      attack_move.weapon->crit_range - attack_move.crit_range_bonus;
  if (attack_roll >= crit_threshold) {
    int multiplier = attack_move.weapon->crit_multiplier +
                     attack_move.crit_multiplier_bonus;
    logger_->Info("Critical hit! Multiplier: {}", multiplier);
    return multiplier;
  }
  return 1;
}

}  // namespace resolver
}  // namespace internal
