// -----------------------------------------------------------------------------
// | @file      attack_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/attack_resolver.h"

#include "internal/combat/event_manager.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/i_entity.h"  // IWYU pragma: keep
#include "internal/logging/log_manager.h"

namespace internal {
namespace resolver {

using combat::AttackMove;
using combat::AttackResult;
using combat::CombatEvent;
using combat::CombatEventContext;
using dice_rolls::Term;
using rules::DamageInstance;

AttackResolver::AttackResolver(const combat::AttackSequence& attack_sequence,
                               CombatEventContext* context)
    : attack_sequence_(attack_sequence),
      logger_(logging::LogManager::GetLogger("attack")),
      context_(context) {
}

AttackResolver::~AttackResolver() {
}

void AttackResolver::ResolveSingleAttack(size_t move_index) {
  if (move_index >= attack_sequence_.attacks.size()) {
    return;
  }
  ResolveAttackMove(attack_sequence_.attacks[move_index]);
}

void AttackResolver::ResolveAttackMove(const AttackMove& attack_move) {
  context_->transient_effects.clear();
  for (const auto& enchantment : attack_move.weapon.enchantments) {
    for (const auto& effect : enchantment.effects) {
      context_->transient_effects.push_back(&effect);
    }
  }
  AttackResult current_result{.attack = &attack_move,
                              .d20_roll = 0,
                              .total_attack_roll = 0,
                              .is_hit = false,
                              .is_crit = false,
                              .crit_multiplier = 1,
                              .damage_instances = {}};

  current_result.d20_roll =
      context_->roller->Roll(Term{.dice_groups = {{1, 20}}});
  logger_->debug("D20 roll: {}", current_result.d20_roll);

  current_result.total_attack_roll =
      current_result.d20_roll + CalculateTotalAttackModifier(attack_move);

  combat::EmitCombatEvent(CombatEvent::AttackRoll, context_);

  int defender_ac = context_->target->GetEffectiveAC();
  logger_->info("{} rolls {} vs AC {}", context_->source->GetName(),
                current_result.total_attack_roll, defender_ac);

  if (current_result.total_attack_roll >= defender_ac) {
    current_result.is_hit = true;

    current_result.crit_multiplier =
        CheckCriticalHit(attack_move, current_result.d20_roll,
                         context_->target->GetFortification());
    current_result.is_crit = current_result.crit_multiplier > 1;

    if (current_result.is_crit) {
      combat::EmitCombatEvent(CombatEvent::CriticalHit, context_);
    }

    combat::EmitCombatEvent(CombatEvent::Hit, context_);

    DamageInstance base_dmg =
        CalculateBaseDamage(attack_move, current_result.crit_multiplier);
    current_result.damage_instances.push_back(base_dmg);

    GatherDamageFromSources(attack_move, &current_result);

    int total_damage = 0;
    for (const auto& dmg : current_result.damage_instances) {
      total_damage += dmg.amount;
    }
    logger_->info("Hit! Total damage: {}", total_damage);
  } else {
    logger_->info("Miss!");
    combat::EmitCombatEvent(CombatEvent::Miss, context_);
  }

  context_->results.push_back(current_result);
}

DamageInstance AttackResolver::CalculateBaseDamage(
    const AttackMove& attack_move, int crit_multiplier) {
  Term damage_term = CalculateTotalDamage(attack_move);
  int damage = context_->roller->Roll(damage_term) * crit_multiplier;
  if (damage < 0) {
    damage = 1;
  }

  DamageInstance instance{.amount = damage, .types = 0, .modifiers = 0};

  instance.types |= static_cast<uint16_t>(attack_move.weapon.damage_type);
  instance.modifiers |=
      static_cast<uint16_t>(attack_move.weapon.damage_modifier);

  return instance;
}

void AttackResolver::GatherDamageFromSources(const AttackMove& attack_move,
                                             AttackResult* result) {
  for (const auto& enchantment : attack_move.weapon.enchantments) {
    for (const auto& source : enchantment.damage_sources) {
      DamageInstance dmg = source.contribute(*context_);
      if (dmg.amount > 0) {
        result->damage_instances.push_back(dmg);
        logger_->debug("{} contributes {} damage", source.name, dmg.amount);
      }
    }
  }
}

Term AttackResolver::CalculateTotalDamage(const AttackMove& attack_move) {
  Term total_damage = attack_move.GetAttackDamage();
  total_damage.AddModifier(attack_sequence_.damage_modifier);
  total_damage.AddModifier(context_->source->GetCurrentStats()
                               .base_stats.attack_bonuses.damage_bonus);

  return total_damage;
}

int AttackResolver::CalculateTotalAttackModifier(
    const AttackMove& attack_move) {
  int total_attack_modifier = attack_move.GetAttackModifier();
  total_attack_modifier += attack_sequence_.attack_modifier;
  total_attack_modifier += context_->source->GetCurrentStats()
                               .base_stats.attack_bonuses.attack_bonus;

  return total_attack_modifier;
}

int AttackResolver::CheckCriticalHit(const AttackMove& attack_move,
                                     const int& attack_roll,
                                     const int& fortification) {
  int crit_threshold =
      attack_move.weapon.crit_range - attack_move.crit_range_bonus;
  if (attack_roll >= crit_threshold) {
    int fortification_roll =
        context_->roller->Roll(Term{.dice_groups = {{1, 100}}});
    if (fortification_roll <= fortification) {
      logger_->info("Critical hit negated by fortification!");
      return 1;
    }

    int multiplier =
        attack_move.weapon.crit_multiplier + attack_move.crit_multiplier_bonus;
    logger_->info("Critical hit! Multiplier: {}", multiplier);
    return multiplier;
  }
  return 1;
}

}  // namespace resolver
}  // namespace internal
