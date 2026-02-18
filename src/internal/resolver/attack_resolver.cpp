// -----------------------------------------------------------------------------
// | @file      attack_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/attack_resolver.h"

#include "internal/combat/combat_events.h"
#include "internal/combat/event_manager.h"
#include "internal/dice_rolls/roller.h"
#include "internal/entities/entity.h"
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

namespace internal {
namespace resolver {

using combat::AttackMove;
using combat::AttackResult;
using combat::CombatEvent;
using combat::CombatEventContext;
using dice_rolls::Roller;
using dice_rolls::Term;
using entities::Entity;
using rules::DamageInstance;

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

std::shared_ptr<CombatEventContext> AttackResolver::ResolveAttack() {
  auto context = std::make_shared<CombatEventContext>();
  context->source = attacker_;
  context->target = defender_;
  context->roller = roller_;

  for (const auto& attack_move : attack_sequence_.attacks) {
    ResolveAttackMove(attack_move, context);
  }
  return context;
}

void AttackResolver::ResolveSingleAttack(
    size_t move_index,
    std::shared_ptr<CombatEventContext> context) {
  if (move_index >= attack_sequence_.attacks.size()) {
    return;
  }
  ResolveAttackMove(attack_sequence_.attacks[move_index], context);
}

void AttackResolver::ResolveAttackMove(
    const AttackMove& attack_move,
    std::shared_ptr<CombatEventContext> context) {
  AttackResult current_result{.attack = &attack_move,
                              .d20_roll = 0,
                              .total_attack_roll = 0,
                              .is_hit = false,
                              .is_crit = false,
                              .crit_multiplier = 1,
                              .damage_instances = {}};

  current_result.d20_roll = roller_->Roll(Term{.dice_groups = {{1, 20}}});
  logger_->Debug("D20 roll: {}", current_result.d20_roll);

  current_result.total_attack_roll =
      current_result.d20_roll + CalculateTotalAttackModifier(attack_move);

  combat::EventManager::Emit(CombatEvent::AttackRoll, context);

  int defender_ac = defender_->GetEffectiveAC();
  logger_->Info("{} rolls {} vs AC {}", attacker_->GetName(),
                current_result.total_attack_roll, defender_ac);

  if (current_result.total_attack_roll >= defender_ac) {
    current_result.is_hit = true;

    current_result.crit_multiplier = CheckCriticalHit(
        attack_move, current_result.d20_roll, defender_->GetFortification());
    current_result.is_crit = current_result.crit_multiplier > 1;

    if (current_result.is_crit) {
      combat::EventManager::Emit(CombatEvent::CriticalHit, context);
    }

    combat::EventManager::Emit(CombatEvent::Hit, context);

    DamageInstance base_dmg =
        CalculateBaseDamage(attack_move, current_result.crit_multiplier);
    current_result.damage_instances.push_back(base_dmg);

    GatherDamageFromSources(attack_move, context, &current_result);

    int total_damage = 0;
    for (const auto& dmg : current_result.damage_instances) {
      total_damage += dmg.amount;
    }
    logger_->Info("Hit! Total damage: {}", total_damage);
  } else {
    logger_->Info("Miss!");
    combat::EventManager::Emit(CombatEvent::Miss, context);
  }

  context->results.push_back(current_result);
}

DamageInstance AttackResolver::CalculateBaseDamage(
    const AttackMove& attack_move, int crit_multiplier) {
  Term damage_term = CalculateTotalDamage(attack_move);
  int damage = roller_->Roll(damage_term) * crit_multiplier;
  if (damage < 0) {
    damage = 1;
  }

  DamageInstance instance{.amount = damage, .types = 0, .modifiers = 0};

  if (attack_move.weapon) {
    instance.types |= static_cast<uint16_t>(attack_move.weapon->damage_type);
    instance.modifiers |=
        static_cast<uint16_t>(attack_move.weapon->damage_modifier);
  }

  return instance;
}

void AttackResolver::GatherDamageFromSources(
    const AttackMove& attack_move,
    std::shared_ptr<CombatEventContext> context, AttackResult* result) {
  if (!attack_move.weapon) {
    return;
  }

  for (const auto& enchantment : attack_move.weapon->enchantments) {
    for (const auto& source : enchantment.damage_sources) {
      DamageInstance dmg = source.contribute(context);
      if (dmg.amount > 0) {
        result->damage_instances.push_back(dmg);
        logger_->Debug("{} contributes {} damage", source.name, dmg.amount);
      }
    }
  }
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
                                     const int& attack_roll,
                                     const int& fortification) {
  if (!attack_move.weapon) {
    return 1;
  }

  int crit_threshold =
      attack_move.weapon->crit_range - attack_move.crit_range_bonus;
  if (attack_roll >= crit_threshold) {
    int fortification_roll = roller_->Roll(Term{.dice_groups = {{1, 100}}});
    if (fortification_roll <= fortification) {
      logger_->Info("Critical hit negated by fortification!");
      return 1;
    }

    int multiplier =
        attack_move.weapon->crit_multiplier + attack_move.crit_multiplier_bonus;
    logger_->Info("Critical hit! Multiplier: {}", multiplier);
    return multiplier;
  }
  return 1;
}

}  // namespace resolver
}  // namespace internal
