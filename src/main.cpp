#include <iostream>

#include "internal/factory/factory.h"

int main() {
    std::cout << "Welcome to the Experimental Tabletop Encounter Simulator!" << std::endl;

    auto player = internal::factory::GetPlayer("character.json");

    auto uktril = internal::factory::MonsterFactory(internal::factory::Monster::Uktril);

    return 0;
}
