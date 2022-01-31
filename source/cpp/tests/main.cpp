#include <future>
#include <juce_events/juce_events.h>

class Application : public juce::JUCEApplicationBase
{
public:
    const juce::String getApplicationName () override // NOLINT
    {
        return {};
    }

    const juce::String getApplicationVersion () override // NOLINT
    {
        return {};
    }

    bool moreThanOneInstanceAllowed () override
    {
        return false;
    }

    void initialise (const juce::String &) override
    {
        _testsResult = std::async (&Application::runTests);
    }

    void shutdown () override
    {
        juce::JUCEApplicationBase::setApplicationReturnValue (
            _testsResult.get () == TestsResult::pass ? 0 : 1);
    }

    void anotherInstanceStarted (const juce::String &) override
    {
    }

    void systemRequestedQuit () override
    {
    }

    void suspended () override
    {
    }

    void resumed () override
    {
    }

    void unhandledException (const std::exception * exception,
                             const juce::String & sourceFile,
                             int lineNumber) override
    {
        juce::Logger::writeToLog ("Unhandled exception in " + sourceFile + "(" +
                                  juce::String (lineNumber) + "): " + exception->what ());
        juce::JUCEApplicationBase::setApplicationReturnValue (1);
    }

private:
    enum class TestsResult
    {
        pass,
        fail,
    };

    static TestsResult runTests ()
    {
        juce::UnitTestRunner runner;
        runner.runAllTests ();

        auto testsResults = TestsResult::pass;

        for (int resultIndex = 0; resultIndex < runner.getNumResults (); ++resultIndex)
            if (auto result = runner.getResult (resultIndex))
                if (result->failures > 0)
                    testsResults = TestsResult::fail;

        juce::JUCEApplicationBase::quit ();
        return testsResults;
    }

    std::future<TestsResult> _testsResult;
};

START_JUCE_APPLICATION (Application)
