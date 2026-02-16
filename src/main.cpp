#include <iostream>

#include "internal/factory/factory.h"
#include "internal/resolver/attack_resolver.h"

int main() {
    std::cout << "Welcome to the Experimental Tabletop Encounter Simulator!" << std::endl;

    auto player = internal::factory::GetPlayer("character.json");
    auto uktril = internal::factory::MonsterFactory(internal::factory::Monster::Uktril);
    auto reishid = internal::factory::MonsterFactory(internal::factory::Monster::Reishid);

    auto roller = std::make_shared<internal::dice_rolls::Roller>();
    auto resolver = std::make_shared<internal::resolver::AttackResolver>(
        player, uktril, 0, roller);

    std::vector<internal::resolver::AttackResolver::AttackResult> results =
        resolver->ResolveAttack();

    for (const auto& result : results) {
      if (!result.is_hit) {
        continue;
      }

      int total_damage = 0;
      for (const auto& dmg_instance : result.damage_instances) {
        total_damage += dmg_instance.amount;
      }
      uktril->TakeDamage(total_damage);
    }

    std::cout << uktril->GetName() << " has "
              << uktril->GetCurrentStats().base_stats.hp << " HP left."
              << std::endl;

    auto resolver2 = std::make_shared<internal::resolver::AttackResolver>(
        reishid, player, 0, roller);

    std::vector<internal::resolver::AttackResolver::AttackResult> results2 =
        resolver2->ResolveAttack();

    for (const auto& result : results2) {
      if (!result.is_hit) {
        continue;
      }
      int total_damage = 0;
      for (const auto& dmg_instance : result.damage_instances) {
        total_damage += dmg_instance.amount;
      }
      player->TakeDamage(total_damage);
    }

    return 0;
}
