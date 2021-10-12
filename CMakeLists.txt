cmake_minimum_required (VERSION 3.20)

include (FetchContent)
fetchcontent_declare (juce GIT_REPOSITORY https://github.com/juce-framework/JUCE
                      GIT_TAG 6.1.2)

fetchcontent_makeavailable (juce)

project (ampify-e2e-testing)

add_subdirectory (app)
