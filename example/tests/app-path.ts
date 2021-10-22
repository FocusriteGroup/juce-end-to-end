import path from 'path';

const configuration = process.env['APP_CONFIGURATION'] || 'Release';
const repoRoot = path.join(__dirname, '..', '..');
const artefactDir = path.join(
  repoRoot,
  'cmake-build/example/app/e2e-example-app_artefacts'
);
const configDir = path.join(artefactDir, configuration);

export const appPath =
  process.env['APP_PATH'] ||
  (process.platform === 'darwin'
    ? path.join(configDir, 'e2e-example-app.app/Contents/MacOS/e2e-example-app')
    : path.join(configDir, 'e2e-example-app.exe'));
