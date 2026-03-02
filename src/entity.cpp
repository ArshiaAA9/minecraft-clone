#include "entity.hpp"

#include <iostream>

void Entity::switchGodmode() {
    std::cout << "godmoding\n";
    if (m_state.godmode == true) m_state.godmode = false;
    else
        m_state.godmode = true;
}
