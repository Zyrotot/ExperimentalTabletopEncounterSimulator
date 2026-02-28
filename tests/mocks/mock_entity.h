// -----------------------------------------------------------------------------
// | @file      mock_entity.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef TESTS_MOCKS_MOCK_ENTITY_H_
#define TESTS_MOCKS_MOCK_ENTITY_H_

#include <gmock/gmock.h>

#include <string>
#include <vector>

#include "src/internal/entities/i_entity.h"
#include "src/internal/positioning/entity_size.h"

class MockEntity : public ettes::entities::IEntity {
 public:
  MOCK_METHOD(uint32_t, GetId, (), (const, override));
  MOCK_METHOD(const std::string&, GetName, (), (const, override));
  MOCK_METHOD(ettes::positioning::EntitySize, GetEntitySize, (),
              (const, override));
  MOCK_METHOD(double, GetSpeedMeters, (), (const, override));
  MOCK_METHOD(const ettes::entities::Stats&, GetCurrentStats, (),
              (const, override));
  MOCK_METHOD(const ettes::combat::AttackSequence&, GetAttackSequence, (int),
              (const, override));
  MOCK_METHOD(const ettes::rules::Alignment&, GetAlignment, (),
              (const, override));

  MOCK_METHOD(int, GetEffectiveAC, (), (const, override));
  MOCK_METHOD(int, GetFortification, (), (const, override));

  MOCK_METHOD(void, TakeDamage, (int), (override));
  MOCK_METHOD(void, Heal, (int), (override));
  MOCK_METHOD(void, AddTempHP, (int), (override));
  MOCK_METHOD(bool, IsAlive, (), (const, override));
  MOCK_METHOD(ettes::entities::Resistances, GetResistances, (),
              (const, override));
  MOCK_METHOD(void, AddDR, (const ettes::rules::DamageReduction&, bool),
              (override));
  MOCK_METHOD(void, RemoveDR, (int, bool), (override));
  MOCK_METHOD(void, ClearAllDR, (bool), (override));

  MOCK_METHOD(bool, HasAbility, (ettes::abilities::AbilityId),
              (const, override));
  MOCK_METHOD(int, GetAbilityStack, (ettes::abilities::AbilityId),
              (const, override));
  MOCK_METHOD(void, IncrementAbilityStack, (ettes::abilities::AbilityId),
              (override));
  MOCK_METHOD(void, DecrementAbilityStack, (ettes::abilities::AbilityId),
              (override));
  MOCK_METHOD(void, SetAbilityStack, (ettes::abilities::AbilityId, int),
              (override));
  MOCK_METHOD(const std::vector<const ettes::combat::Effect*>&,
              GetActiveEffects, (), (const, override));
  MOCK_METHOD(const std::vector<const ettes::combat::Effect*>&,
              GetEffectsForEvent, (ettes::combat::CombatEvent),
              (const, override));
  MOCK_METHOD(void, BuildActiveEffects, (), (override));
};

#endif  // TESTS_MOCKS_MOCK_ENTITY_H_
