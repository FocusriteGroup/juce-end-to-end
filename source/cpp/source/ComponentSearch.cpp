#include <focusrite/e2e/ComponentSearch.h>

namespace focusrite::e2e
{
static constexpr auto testId = "test-id";
static constexpr auto windowId = "window-id";

std::set<juce::Component *> ComponentSearch::_rootComponents { };

std::vector<juce::Component *>
getDirectDescendantsMatching (juce::Component & parent,
                              const std::function<bool (juce::Component &)> & predicate)
{
    auto children = parent.getChildren ();
    children.removeIf ([&] (auto && child) { return child == nullptr || ! predicate (*child); });
    return {children.begin (), children.end ()};
}

juce::Component * matchChildComponent (juce::Component & component,
                                       const std::function<bool (juce::Component &)> & predicate,
                                       int & skip)
{
    jassert (skip >= 0);

    auto matchingChildren = getDirectDescendantsMatching (component, predicate);

    if (skip < int (matchingChildren.size ()))
        return matchingChildren [skip];

    skip -= int (matchingChildren.size ());

    for (auto * child : component.getChildren ())
    {
        if (child == nullptr)
            continue;

        if (auto * foundComponent = matchChildComponent (*child, predicate, skip))
            return foundComponent;
    }

    return nullptr;
}

juce::Component * findChildComponent (juce::Component & component,
                                      const std::function<bool (juce::Component &)> & predicate,
                                      int skip = 0)
{
    return matchChildComponent (component, predicate, skip);
}

std::vector<juce::TopLevelWindow *> getTopLevelWindows ()
{
    std::vector<juce::TopLevelWindow *> windows;

    for (int windowIndex = 0; windowIndex < juce::TopLevelWindow::getNumTopLevelWindows ();
         ++windowIndex)
        if (auto * window = juce::TopLevelWindow::getTopLevelWindow (windowIndex))
            windows.push_back (window);

    return windows;
}

bool componentHasMatchingProperty (const juce::Component & component,
                                   const juce::String & pattern,
                                   const juce::String & propertyName)
{
    const auto componentTestId =
        component.getProperties ().getWithDefault (propertyName, {}).toString ();
    const auto componentId = component.getComponentID ();

    return componentTestId.matchesWildcard (pattern, false) ||
           componentId.matchesWildcard (pattern, false);
}

bool componentHasId (const juce::Component & component, const juce::String & idPattern)
{
    return componentHasMatchingProperty (component, idPattern, testId);
}

bool windowHasId (const juce::TopLevelWindow & window, const juce::String & idPattern)
{
    return componentHasMatchingProperty (window, idPattern, windowId);
}

std::function<bool (juce::Component &)> createComponentMatcher (const juce::String & componentId)
{
    return [componentId] (auto && component) -> bool
    {
        return componentHasId (component, componentId) && component.isVisible () &&
               component.isShowing ();
    };
}

juce::TopLevelWindow * ComponentSearch::findWindowWithId (const juce::String & id)
{
    auto topWindows = getTopLevelWindows ();
    if (topWindows.empty ())
        return nullptr;

    if (id.isEmpty ())
        return topWindows.front ();

    auto it = std::find_if (topWindows.begin (),
                            topWindows.end (),
                            [&] (auto && window) { return windowHasId (*window, id); });

    return it == topWindows.end () ? nullptr : *it;
}

int ComponentSearch::countChildComponents (const juce::Component & root,
                                           const juce::String & componentId)
{
    const auto predicate = createComponentMatcher (componentId);

    return std::accumulate (root.getChildren ().begin (),
                            root.getChildren ().end (),
                            0,
                            [=] (auto && total, auto && child)
                            {
                                if (predicate (*child))
                                    ++total;

                                total += countChildComponents (*child, componentId);

                                return total;
                            });
}

juce::Component * ComponentSearch::findWithId (const juce::String & componentId, int skip)
{
    auto componentIds = juce::StringArray::fromTokens (componentId, "/", "");
    if (componentIds.isEmpty ())
        return nullptr;

    auto * component = findComponent (createComponentMatcher (componentIds [0]), skip);
    componentIds.remove (0);

    for (auto && id : componentIds)
    {
        if (component == nullptr)
            return nullptr;

        component = findChildComponent (*component, createComponentMatcher (id), 0);
    }

    return component;
}

void ComponentSearch::setTestId (juce::Component & component, const juce::String & id)
{
    component.getProperties ().set (testId, id);
}

void ComponentSearch::setWindowId (juce::TopLevelWindow & window, const juce::String & id)
{
    window.getProperties ().set (windowId, id);
}

void ComponentSearch::addRootComponent(juce::Component * rootComponent)
{
    _rootComponents.insert(rootComponent);
}

void ComponentSearch::removeRootComponent(juce::Component * rootComponent)
{
    _rootComponents.erase(rootComponent);
}

juce::Component * ComponentSearch::findComponent (const std::function<bool (juce::Component &)> & predicate,
                                 int skip)
{
    for (auto & window : getTopLevelWindows ())
        if (auto * component = findChildComponent (*window, predicate, skip))
            return component;

    for (auto & rootComponent : _rootComponents)
    { 
        if (auto * component = findChildComponent (*rootComponent, predicate, skip))
        {
            return component;
        }
    }

    return nullptr;
}

std::unique_ptr<juce::DynamicObject> createComponentTree(juce::Component* component)
{
    auto element = std::make_unique<juce::DynamicObject> ();
    if (! component)
        return element;

    element->setProperty ("id", component->getComponentID());
    element->setProperty ("name", component->getName());
    element->setProperty ("visible", component->isVisible());
    element->setProperty ("enabled", component->isEnabled());
    if (const auto* label = dynamic_cast<juce::Label*>(component))
    {
        element->setProperty ("text", label->getText());
    }
    if (const auto* textEditor = dynamic_cast<juce::TextEditor*>(component))
    {
        element->setProperty ("text", textEditor->getText());
    }
    if (const auto* slider = dynamic_cast<juce::Slider*>(component))
    {
        element->setProperty ("value", slider->getValue());
    }
    
    juce::Array<juce::var> children;
    for (const auto& child: component->getChildren())
    {
        children.add (createComponentTree (child).release());
    }

    element->setProperty ("children", children);

    return element;
}

juce::var ComponentSearch::dumpComponentTree(juce::Component * parent)
{
    juce::Array<juce::var> components;
    if (parent)
    {
        components.add (createComponentTree (parent).release ());
    }
    else 
    {
        const auto topLevelWindows = getTopLevelWindows ();
        std::set<juce::Component * > topLevelComponents { 
            topLevelWindows.begin (), topLevelWindows.end () };
        topLevelComponents.insert (_rootComponents.begin (), _rootComponents.end ());

        for (const auto& component: topLevelComponents)
        {
            components.add (createComponentTree (component).release());
        }
    }
    return components;
}

juce::String ComponentSearch::getComponentPath(juce::Component * component)
{
    const auto topLevelWindows = getTopLevelWindows ();
    std::set<juce::Component * > topLevelComponents { 
        topLevelWindows.begin (), topLevelWindows.end () };
    topLevelComponents.insert (_rootComponents.begin (), _rootComponents.end ());

    juce::String path;
    while (component && 
        topLevelComponents.find(component) == topLevelComponents.end()) 
    {
        path = component->getComponentID() + 
            ((!path.isEmpty()) ? juce::String("/") + path : juce::String());
        component = component->getParentComponent();
    }
    return path;
}

}
