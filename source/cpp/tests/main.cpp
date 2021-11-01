#include <juce_core/juce_core.h>

int main ()
{
    juce::UnitTestRunner runner;
    runner.runAllTests ();

    for (int resultIndex = 0; resultIndex < runner.getNumResults (); ++resultIndex)
        if (auto result = runner.getResult (resultIndex))
            if (result->failures > 0)
                return 1;

    return 0;
}