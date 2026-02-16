// -----------------------------------------------------------------------------
// | @file      attack_resolver.cpp
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#include "internal/resolver/attack_resolver.h"

#include "internal/dice_rolls/roller.h"  // IWYU pragma: keep
#include "internal/entities/entity.h"    // IWYU pragma: keep
#include "internal/logging/log_manager.h"
#include "internal/logging/logger.h"

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
  logger_->SetLogLevel(logging::LogLevel::DEBUG);
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
  CombatContext context{
      .attacker = attacker_,
      .target = defender_,
      .attack = &attack_move,
      .roller = roller_,
      .d20_roll = 0,
      .total_attack_roll = 0,
      .is_hit = false,
      .is_crit = false,
      .crit_multiplier = 1,
      .damage_instances = {}};

  context.d20_roll = roller_->Roll(Term{.dice_groups = {{1, 20}}});
  logger_->Debug("D20 roll: {}", context.d20_roll);

  context.total_attack_roll =
      context.d20_roll + CalculateTotalAttackModifier(attack_move);

  EmitEvent(CombatEvent::AttackRoll, &context);

  int defender_ac = defender_->GetEffectiveAC();
  logger_->Info("{} rolls {} vs AC {}", attacker_->GetName(),
                context.total_attack_roll, defender_ac);

  if (context.total_attack_roll >= defender_ac) {
    context.is_hit = true;

    context.crit_multiplier =
        CheckCriticalHit(attack_move, context.d20_roll);
    context.is_crit = context.crit_multiplier > 1;

    if (context.is_crit) {
      EmitEvent(CombatEvent::CriticalHit, &context);
    }

    EmitEvent(CombatEvent::Hit, &context);

    DamageInstance base_dmg =
        CalculateBaseDamage(attack_move, context.crit_multiplier);
    context.damage_instances.push_back(base_dmg);

    GatherDamageFromSources(attack_move, &context);

    int total_damage = 0;
    for (const auto& dmg : context.damage_instances) {
      total_damage += dmg.amount;
    }
    logger_->Info("Hit! Total damage: {}", total_damage);

    // TODO(zyrotot): Trigger after resistances, weaknesses, etc. are applied
    // EmitEvent(CombatEvent::DealDamage, &context);

  } else {
    EmitEvent(CombatEvent::Miss, &context);
  }

  return context;
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
    const AttackMove& attack_move, CombatContext* context) {
  if (!attack_move.weapon) {
    return;
  }

  for (const auto& enchantment : attack_move.weapon->enchantments) {
    for (const auto& source : enchantment.damage_sources) {
      DamageInstance dmg = source.contribute(*context, *roller_);
      if (dmg.amount > 0) {
        context->damage_instances.push_back(dmg);
        logger_->Debug("{} contributes {} damage", source.name, dmg.amount);
      }
    }
  }
}

void AttackResolver::EmitEvent(CombatEvent event, CombatContext* context) {
  if (!context || !context->attack || !context->attack->weapon) {
    return;
  }

  for (const auto& enchantment : context->attack->weapon->enchantments) {
    for (const auto& effect : enchantment.effects) {
      if (effect.trigger == event) {
        logger_->Debug("Triggering {} effect", effect.name);
        effect.on(*context);
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
                                     const int& attack_roll) {
  if (!attack_move.weapon) {
    return 1;
  }

  int crit_threshold =
      attack_move.weapon->crit_range - attack_move.crit_range_bonus;
  if (attack_roll >= crit_threshold) {
    int multiplier =
        attack_move.weapon->crit_multiplier + attack_move.crit_multiplier_bonus;
    logger_->Info("Critical hit! Multiplier: {}", multiplier);
    return multiplier;
  }
  return 1;
}

}  // namespace resolver
}  // namespace internal
