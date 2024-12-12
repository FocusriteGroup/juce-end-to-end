include (FetchContent)
fetchcontent_declare (juce GIT_REPOSITORY https://github.com/juce-framework/JUCE
                      GIT_TAG 7.0.9 GIT_SUBMODULES "")

fetchcontent_makeavailable (juce)
