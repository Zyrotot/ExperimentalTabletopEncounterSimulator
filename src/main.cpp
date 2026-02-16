#include <iostream>

#include "internal/factory/factory.h"
#include "internal/resolver/attack_resolver.h"

int main() {
    std::cout << "Welcome to the Experimental Tabletop Encounter Simulator!" << std::endl;

    auto player = internal::factory::GetPlayer("character.json");
    auto uktril = internal::factory::MonsterFactory(internal::factory::Monster::Uktril);

    auto roller = std::make_shared<internal::dice_rolls::Roller>();
    auto resolver = std::make_shared<internal::resolver::AttackResolver>(player, uktril, 0, roller);

    std::vector<internal::resolver::AttackResolver::AttackResult> results = resolver->ResolveAttack();

    for (const auto& result : results) {
        uktril->TakeDamage(result.damage);
    }

    std::cout << uktril->GetName() << " has "
              << uktril->GetCurrentStats().base_stats.hp << " HP left."
              << std::endl;

    return 0;
}
