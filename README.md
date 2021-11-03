# JUCE End to End test framework

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

[![Focusrite-Novation](https://circleci.com/gh/Focusrite-Novation/juce-end-to-end.svg?style=shield&circle-token=11bdc9a65f58ecac7388d385b1ed052c848c4924)](https://app.circleci.com/pipelines/github/Focusrite-Novation/juce-end-to-end)

[![Platform](https://img.shields.io/static/v1?label=Platform&message=macOS%20%7C%20windows&color=pink&style=flat)](./documentation/building.md)

[![Language](https://img.shields.io/static/v1?label=Language&message=C%2B%2B&color=orange&style=flat)](./documentation/building.md)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=Clang%20Format&color=pink&style=flat)](https://clang.llvm.org/docs/ClangFormat.html)

[![Language](https://img.shields.io/static/v1?label=Language&message=TypeScript&color=orange&style=flat)](./documentation/building.md)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=Prettier&color=pink&style=flat)](https://prettier.io)

[![Language](https://img.shields.io/static/v1?label=Language&message=CMake&color=orange&style=flat)](https://www.cmake.org)
[![Code Style](https://img.shields.io/static/v1?label=Code%20Style&message=CMake%20Format&color=pink&style=flat)](https://github.com/cheshirekow/cmake_format)

## What is it?

This package provides a mechanism to end-to-end test a JUCE application

## Prerequisites

- [CMake](https://www.cmake.org). Must be 3.18 or higher. Normally just get the latest version
  - CMake is used as the build system, as we as a pseudo package manager. It is the simplest way to use JE2E
- [Node](https://nodejs.org/en/). Should be 14.x, with npm 6.14.x
  - Node is used to manage the build and test process. You can manually execute any of the normal commands in Cmake and your compiler if you don't wish to use Node
- [JUCE](https://juce.com). Should be >= 6.x
  - This project is designed to integrate with JUCE. We use large parts of the JUCE mainline. To make this project work, JUCE should be available on your compile path

## Integration guide

To integrate into your project, please read the [Integration Guide](./documentation/integration-guide.md)

## Building

You can build on the command line, or using an IDE (We support _XCode_, _CLion_ and _VS2019_ currently). See [here](./documentation/building.md) for how to build.

## CI

Our ci is [CircleCI](https://www.circleci.com). The config for circle is [here](./.circleci/config.yml)

## Code formatting

We use a variety of code formatting tools. Please make sure you have these installed on your system to keep the codebase styling consistent.

- [C++](./documentation/cplusplus.md)
- [Javascript](./documentation/javascript.md)
- [CMake](./documentation/cmake.md)

## Scripts

We have a variety of scripts available in our package.json. [Learn more](./documentation/scripts.md)
