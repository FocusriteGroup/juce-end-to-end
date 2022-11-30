# C++

For _C++_ we use _clang-format_.

## Installation (XCode)

```sh
brew install clang-format
```

## Installation (CLion)

If you are using _CLion_, code formatting is automatically provided by the IDE, because we have a `.clang-format` file in the root of the directory.
In order to enable this behaviour, you need to

Use the save actions plug-in to enable format on save.
The settings are:

- 'Activate save action on save' should be enabled
- Formatting actions: 'Re-format file' should be enabled
- File path inclusions should have: modules /_and source/_
- File path exclusions should have: CMakeLists.txt and lib/*

## Installation (Visual Studio 2019)

_Visual Studio 2019_ has built in clang-format support, but it is a very old version. To use a more modern version, you need to install the [clang tools](http://llvm.org/releases/) then add `%LLVM%\bin` to your path (`%LLVM%` is where you installed to)
