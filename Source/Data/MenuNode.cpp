#include "Data/MenuNode.h"

MenuNode* MenuNode::addChild (std::unique_ptr<MenuNode> child)
{
    MenuNode* childPtr = child.get();

    // Check for any duplicated shortcut keys in the children before adding
    // Think of this as a configuration error if things go wrong - just for devs, safe to kill the program
    for (auto& c : children)
    {
        if (c->navShortcut == child->navShortcut)
        {
            std::cerr << "!!! CONFIGURATION ERROR !!!" << std::endl;
            std::cerr << "Attempting to add pre-existing MenuNode shortcut : " << child->navShortcut.getTextDescription() << std::endl;
            std::abort();
        }
    }

    children.push_back (std::move (child));
    return childPtr;
}
