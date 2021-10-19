const path = require('path');
const fs = require('fs');
const {execSync} = require('child_process');
const glob = require('glob');
const tar = require('tar');
const paths = require('./lib/paths');
const platform = require('./lib/platform');

const skipClean = process.env.SKIP_CLEAN || false;
const generator =
  process.env.GENERATOR ||
  (platform.isMac() ? 'Xcode' : 'Visual Studio 16 2019');

const removeDirectory = (path) => {
  if (fs.existsSync(path)) {
    fs.rmdirSync(path, {recursive: true});
  }
};

const createDirectory = (path) => {
  if (!fs.existsSync(path)) {
    fs.mkdirSync(path, {recursive: true});
  }
};

const createBuildFolder = () => {
  if (skipClean) {
    return;
  }

  removeDirectory(paths.build);
  createDirectory(paths.build);
};

const generateBuildSystem = () => {
  execSync(
    `cmake -DAMPIFY_E2E_FETCH_JUCE=ON -G "${generator}" -S "${paths.source}" -B "${paths.build}"`,
    {
      stdio: 'inherit',
    }
  );
};

const build = (configuration) => {
  execSync(`cmake --build "${paths.build}" --config "${configuration}"`, {
    stdio: 'inherit',
  });
};

const copyLibrary = (configuration) => {
  fs.copyFileSync(
    paths.libraryBuild(configuration),
    paths.libraryInstall(configuration)
  );
};

const copyHeaders = () => {
  const headers = glob.sync('**/*.h', {
    cwd: paths.includeSource,
  });

  headers.forEach((header) => {
    const sourceFile = path.join(paths.includeSource, header);
    const destFile = path.join(paths.include, header);
    createDirectory(path.dirname(destFile));
    fs.copyFileSync(sourceFile, destFile);
  });
};

const copyCmakeConfig = () => {
  fs.copyFileSync(paths.cmakeConfigSource, paths.cmakeConfigDestination);
};

const createArchive = () => {
  tar.create(
    {
      gzip: true,
      file: paths.archive,
      sync: true,
      cwd: paths.install,
    },
    ['include/', 'lib/']
  );

  console.log(`Wrote output file to ${paths.archive}`);
};

const createInstallation = () => {
  removeDirectory(paths.install);
  createDirectory(paths.install);
  createDirectory(paths.include);
  createDirectory(paths.lib);
  createDirectory(paths.cmake);

  copyLibrary('Debug');
  copyLibrary('Release');
  copyHeaders();
  copyCmakeConfig();
  createArchive();
};

const main = () => {
  createBuildFolder();
  generateBuildSystem();
  build(paths.build, 'Debug');
  build(paths.build, 'Release');
  createInstallation();
};

main();
