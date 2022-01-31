#include <focusrite/e2e/ComponentSearch.h>
#include <future>
#include <juce_core/juce_core.h>

namespace focusrite::e2e
{
template <typename Task>
void runOnMessageQueue (Task task)
{
    juce::WaitableEvent event;

    juce::MessageManager::callAsync (
        [&]
        {
            task ();
            event.signal ();
        });

    event.wait ();
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
            Test {"Finds nested components with slashes",
                  [=] { findsNestedComponentsWithSlashes (); }},
        };

        for (auto && test : tests)
        {
            runOnMessageQueue (
                [=]
                {
                    beginTest (test.name);
                    test.entry ();
                });
        }
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

    void findsNestedComponentsWithSlashes ()
    {
        constexpr auto componentAId = "component-a";
        constexpr auto componentBId = "component-b";
        constexpr auto componentCId = "component-c";

        constexpr auto genericId = "generic";

        juce::Component componentA;
        juce::Component componentB;
        juce::Component componentC;

        componentA.setComponentID (componentAId);
        componentB.setComponentID (componentBId);
        componentC.setComponentID (componentCId);

        componentA.addAndMakeVisible (componentB);
        componentA.addAndMakeVisible (componentC);

        juce::Component genericA;
        juce::Component genericB;
        juce::Component genericC;

        genericA.setComponentID (genericId);
        componentA.addAndMakeVisible (genericA);

        genericB.setComponentID (genericId);
        componentB.addAndMakeVisible (genericB);

        genericC.setComponentID (genericId);
        componentC.addAndMakeVisible (genericC);

        juce::TopLevelWindow window ("window", true);
        window.addAndMakeVisible (componentA);
        window.setVisible (true);

        expect (ComponentSearch::findWithId ("component-a/generic") == &genericA);
        expect (ComponentSearch::findWithId ("component-b/generic") == &genericB);
        expect (ComponentSearch::findWithId ("component-c/generic") == &genericC);
        expect (ComponentSearch::findWithId ("component-a/component-b/*") == &genericB);
        expect (ComponentSearch::findWithId ("component-a/component-c/*") == &genericC);
        expect (ComponentSearch::findWithId ("component-a/*/generic") == &genericB);
    }
};

[[maybe_unused]] static ComponentSearchTests componentSearchTests;
}
