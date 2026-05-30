#include <hugin/diagnostics.hpp>

namespace hugin {

diagnostic_error::diagnostic_error(std::string const &message)
  : std::runtime_error(message)
{
}

}  // namespace hugin
