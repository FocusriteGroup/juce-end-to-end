For _Javascript_, _Typescript_ and _HTML_ and other similar languages, we use the Ampify Prettier config. 

## Installation
[TODO]

## Format on save in VScode

Install the [prettier extension](https://marketplace.visualstudio.com/items?itemName=esbenp.prettier-vscode). 

You will then need to update your VSCode [settings.json](https://vscode.readthedocs.io/en/latest/getstarted/settings/) file, to include the following:
```
"[typescriptreact]": {
    "editor.tabSize": 2,
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "esbenp.prettier-vscode"
},
"[javascript]": {
    "editor.tabSize": 2,
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "esbenp.prettier-vscode"
},
"[typescript]": {
    "editor.tabSize": 2,
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "esbenp.prettier-vscode"
},
"[json]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
},
"[html]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
},
```