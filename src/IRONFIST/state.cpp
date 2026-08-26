#include <IRONFIST/state.h>

namespace ironfist {

namespace state {

State& Get() {
    static State state;
    return state;
}

} // namespace state

} // namespace ironfist
