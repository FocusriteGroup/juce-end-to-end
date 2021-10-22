include (FetchContent)

fetchcontent_declare (
  Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.0.0-preview3)

fetchcontent_makeavailable (Catch2)
