import path from 'path';
import fs from 'fs';
import {execSync} from 'child_process';
import glob from 'glob';
import tar from 'tar';
import paths from './lib/paths';
import {removeDirectory, createDirectory} from './lib/directory';
import {Configuration} from './lib/configuration';

const build = (configuration: Configuration) => {
  execSync(`cmake --build "${paths.build}" --config "${configuration}"`, {
    stdio: 'inherit',
  });
};

const copyLibrary = (configuration: Configuration) => {
  const library = paths.library(configuration);
  const filename = path.basename(library);
  fs.copyFileSync(library, path.join(paths.lib, filename));
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
  build('Debug');
  build('Release');
  createInstallation();
};

main();
