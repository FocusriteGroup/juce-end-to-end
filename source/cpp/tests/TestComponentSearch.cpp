#include <focusrite/e2e/ComponentSearch.h>
#include <future>
#include <juce_core/juce_core.h>

namespace focusrite::e2e
{
template <typename Task>
void runOnMessageQueue (Task task)
{
    auto messageManager = juce::MessageManager::getInstance ();
    jassert (messageManager != nullptr);

    juce::MessageManager::callAsync (
        [task, messageManager]
        {
            task ();
            messageManager->stopDispatchLoop ();
        });

    messageManager->runDispatchLoop ();
}

class ComponentSearchTests final : public juce::UnitTest
{
public:
    ComponentSearchTests () noexcept
        : juce::UnitTest ("ComponentSearch")
    {
    }

    void runTest () override
    {
        struct Test
        {
            juce::String name;
            std::function<void ()> entry;
        };

        auto tests = {
            Test {"Finds component with component ID", [=] { findsComponentWithId (); }},
            Test {"Finds component with test ID", [=] { findsComponentWithTestId (); }},
        };

        for (auto && test : tests)
        {
            beginTest (test.name);
            runOnMessageQueue (test.entry);
        }

        juce::MessageManager::deleteInstance ();
    }

    void findsComponentWithId ()
    {
        constexpr auto componentAId = "component-a";
        constexpr auto componentBId = "component-b";
        constexpr auto componentCId = "component-c";

        juce::TopLevelWindow window ("window", true);
        juce::Component componentA;
        juce::Component componentB;
        juce::Component componentC;

        componentA.setComponentID (componentAId);
        componentB.setComponentID (componentBId);
        componentC.setComponentID (componentCId);

        componentA.addAndMakeVisible (componentB);
        componentA.addAndMakeVisible (componentC);
        window.addAndMakeVisible (componentA);
        window.setVisible (true);

        expect (ComponentSearch::findWithId (componentAId) == &componentA);
        expect (ComponentSearch::findWithId (componentBId) == &componentB);
        expect (ComponentSearch::findWithId (componentCId) == &componentC);
    }

    void findsComponentWithTestId ()
    {
        constexpr auto componentAId = "component-a";
        constexpr auto componentBId = "component-b";
        constexpr auto componentCId = "component-c";

        juce::TopLevelWindow window ("window", true);
        juce::Component componentA;
        juce::Component componentB;
        juce::Component componentC;

        ComponentSearch::setTestId (componentA, componentAId);
        ComponentSearch::setTestId (componentB, componentBId);
        ComponentSearch::setTestId (componentC, componentCId);

        componentA.addAndMakeVisible (componentB);
        componentA.addAndMakeVisible (componentC);
        window.addAndMakeVisible (componentA);
        window.setVisible (true);

        expect (ComponentSearch::findWithId (componentAId) == &componentA);
        expect (ComponentSearch::findWithId (componentBId) == &componentB);
        expect (ComponentSearch::findWithId (componentCId) == &componentC);
    }
};

[[maybe_unused]] static ComponentSearchTests componentSearchTests;

}