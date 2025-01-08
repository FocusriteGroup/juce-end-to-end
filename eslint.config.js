const eslint = require('@typescript-eslint/eslint-plugin');
const parser = require('@typescript-eslint/parser');
const {configs} = require('@eslint/js');

module.exports = [
  configs.recommended,
  {ignores: ['**/node_modules/*', '**/dist/*']},
  {
    rules: {
      'no-undef': 'off',
      'no-unused-vars': 'off',
    },
    languageOptions: {
      parser: parser,
    },
    plugins: {eslint},
    files: ['**/*.ts', '**/*.tsx', '**/*.js'],
  },
];
