#pragma once

#include <cstddef>  // size_t
#include <cstdint>  // uint8_t

namespace databento {
// An abstract class for writable objects to allow for runtime polymorphism
// around DBN encoding.
class IWritable {
 public:
  virtual ~IWritable() = default;

  virtual void Write(const std::uint8_t* buffer, std::size_t length) = 0;
};
}  // namespace databento
