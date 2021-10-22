import {execSync} from 'child_process';
import * as platform from './lib/platform';
import paths from './lib/paths';
import {removeDirectory, createDirectory} from './lib/directory';

const skipClean = process.env.SKIP_CLEAN || false;

const generator =
  process.env.GENERATOR ||
  (platform.isMac() ? 'Xcode' : 'Visual Studio 16 2019');

const generateBuildSystem = () => {
  execSync(
    `cmake -DAMPIFY_E2E_MAKE_TESTS=ON -DAMPIFY_E2E_FETCH_JUCE=ON -G "${generator}" -S "${paths.source}" -B "${paths.build}"`,
    {
      stdio: 'inherit',
    }
  );
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
