// -----------------------------------------------------------------------------
// | @file      enchantment_library.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_
#define SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_

namespace internal {
namespace items {

class Enchantment;

Enchantment CreateFlamingEnchantment();
Enchantment CreateFlamingExplosionEnchantment();
Enchantment CreateDissonantEnchantment();
Enchantment CreateVampiricEnchantment();
Enchantment CreateDrainingEnchantment();
Enchantment CreateProfaneEnchantment();

}  // namespace items
}  // namespace internal

#endif  // SRC_INTERNAL_ITEMS_ENCHANTMENT_LIBRARY_H_
