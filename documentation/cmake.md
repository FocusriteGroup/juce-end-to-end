# CMake

For _CMake_ formatting we use [_cmake-format_](https://github.com/cheshirekow/cmake_format)

## Installation

```sh
pip install cmakelang
```

## Format on save in VScode

To have format on save in VSCode, you can install the [VSCode extension](<https://marketplace.visualstudio.com/items>? itemName=cheshirekow.cmake-format).

You will then need to update your VSCode [settings.json](https://code.visualstudio.com/docs/getstarted/settings#_settingsjson) file, to include the following:

```json
"files.associations": {
    "CMakeLists.txt": "cmake",
    "*.cmake": "cmake"
},
"[cmake]": {
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "cheshirekow.cmake-format"
},
```
