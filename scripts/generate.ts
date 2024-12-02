import {execSync} from 'child_process';
import * as platform from './lib/platform';
import paths from './lib/paths';
import {removeDirectory, createDirectory} from './lib/directory';

const skipClean = process.env['SKIP_CLEAN'] || false;

const generator =
  process.env['GENERATOR'] ||
  (platform.isMac() ? 'Xcode' : 'Visual Studio 16 2019');

const generateBuildSystem = () => {
  const args = [
    'cmake',
    '-DFOCUSRITE_E2E_MAKE_TESTS=ON',
    '-DFOCUSRITE_E2E_FETCH_JUCE=ON',
    '-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"',
    '-G',
    `"${generator}"`,
    '-S',
    `"${paths.source}"`,
    '-B',
    `"${paths.build}"`,
  ];

  execSync(args.join(' '), {
    stdio: 'inherit',
  });
};

const createBuildFolder = () => {
  if (skipClean) {
    return;
  }

  removeDirectory(paths.build);
  createDirectory(paths.build);
};

createBuildFolder();
generateBuildSystem();
