// -----------------------------------------------------------------------------
// | @file      i_factory.h
// | @author    Zyrotot
// | @project   ETTES (2026)
// -----------------------------------------------------------------------------

#ifndef SRC_INTERNAL_FACTORY_I_FACTORY_H_
#define SRC_INTERNAL_FACTORY_I_FACTORY_H_

#include <memory>

#include "internal/entities/i_entity.h"

namespace ettes {
namespace factory {

class IFactory {
 public:
  virtual ~IFactory() = default;

  virtual std::shared_ptr<entities::IEntity> CreatePlayer() const = 0;
  virtual std::shared_ptr<entities::IEntity> CreateMonster() const = 0;
};

}  // namespace factory
}  // namespace ettes

#endif  // SRC_INTERNAL_FACTORY_I_FACTORY_H_
