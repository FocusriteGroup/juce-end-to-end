# JUCE End to End

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0) [![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md) [![CircleCI](https://dl.circleci.com/status-badge/img/gh/FocusriteGroup/juce-end-to-end/tree/main.svg?style=shield)](https://dl.circleci.com/status-badge/redirect/gh/FocusriteGroup/juce-end-to-end/tree/main) [![Platform](https://img.shields.io/static/v1?label=Platform&message=macOS%20%7C%20windows&color=pink&style=flat)](./documentation/building.md)

[![Language](https://img.shields.io/static/v1?label=Language&message=C%2B%2B&color=orange&style=flat)](./documentation/building.md)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=Clang%20Format&color=pink&style=flat)](https://clang.llvm.org/docs/ClangFormat.html)

[![Language](https://img.shields.io/static/v1?label=Language&message=TypeScript&color=orange&style=flat)](./documentation/building.md)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=Prettier&color=pink&style=flat)](https://prettier.io)

[![Language](https://img.shields.io/static/v1?label=Language&message=CMake&color=orange&style=flat)](https://www.cmake.org)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=CMake%20Format&color=pink&style=flat)](https://github.com/cheshirekow/cmake_format)

A framework for end-to-end testing JUCE applications using JavaScript.

## Prerequisites

- [JUCE](https://juce.com) 6 or later
- [CMake](https://cmake.org) 3.18 or higher

## Integration guide

See the full [Integration Guide](./documentation/integration-guide.md) for a detailed walkthrough - the major steps are:

1. Add the `focusrite-e2e` library to your JUCE application using CMake
1. Write a `TestCentre` to allow JavaScript to communicate with your app
1. Create an `AppConnection` object in your favourite JavaScript test framework and use its various methods to test your app

See the [example app](./example/) for an example of how to integrate this framework in your JUCE app.

Watch Joe's ADC talk for an even more detailed explanation of the framework, and to see it in action testing Ampify Studio!

[![Joe's ADC Talk](https://img.youtube.com/vi/3gi7CO71414/0.jpg)](https://www.youtube.com/watch?v=3gi7CO71414)

## Continuous Integration

Our Continuous Integration is [CircleCI](https://www.circleci.com). The config for circle is [here](./.circleci/config.yml)

## Code formatting

We use a variety of code formatting tools. Please make sure you have these installed on your system to keep the codebase styling consistent.

- [C++](./documentation/cplusplus.md)
- [JavaScript](./documentation/javascript.md)
- [CMake](./documentation/cmake.md)

## Scripts

We have a variety of scripts available in our package.json.
