#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace focusrite::e2e
{
class ComponentSearch
{
public:
    static juce::Component * findWithId (const juce::String & componentId, int skip = 0);
    static juce::TopLevelWindow * findWindowWithId (const juce::String & windowId = {});

    static int countChildComponents (const juce::Component & parent,
                                     const juce::String & matchingId);

    static void setTestId (juce::Component & component, const juce::String & id);
    static void setWindowId (juce::TopLevelWindow & window, const juce::String & id);

    static void addRootComponent(juce::Component * rootComponent);
    static void removeRootComponent(juce::Component * rootComponent);

    static juce::var dumpComponentTree(juce::Component * parent = nullptr);
    static juce::String getComponentPath(juce::Component * component);
private:
    static juce::Component * findComponent (const std::function<bool (juce::Component &)> & predicate,
                                    int skip = 0);

    static std::set<juce::Component *> _rootComponents;
};

}
