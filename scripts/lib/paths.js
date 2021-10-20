const path = require('path');
const platform = require('./platform');

const source = process.env.E2E_SOURCE || process.cwd();
const build = path.join(source, 'cmake-build');

const install = path.join(build, 'installation');
const include = path.join(install, 'include');
const lib = path.join(install, 'lib');
const cmake = path.join(lib, 'cmake');

const configFilename = 'AmpifyE2EConfig.cmake';
const cmakeConfigSource = path.join(source, 'cmake', configFilename);
const cmakeConfigDestination = path.join(cmake, configFilename);

const includeSource = path.join(source, 'app', 'include');

const libraryFilename = (configuration) => {
  const extension = platform.isMac() ? 'a' : 'lib';
  const postFix = configuration === 'Debug' ? 'd' : '';
  const prefix = platform.isMac() ? 'lib' : '';
  return `${prefix}ampify-e2e${postFix}.${extension}`;
};

const libraryBuild = (configuration) =>
  path.join(build, 'app', configuration, libraryFilename(configuration));

const libraryInstall = (configuration) =>
  path.join(lib, libraryFilename(configuration));

const archive = path.join(
  install,
  `ampify-e2e-${platform.currentPlatform()}.tar.gz`
);

module.exports = {
  source,
  build,
  install,
  include,
  lib,
  cmake,
  cmakeConfigSource,
  cmakeConfigDestination,
  includeSource,

  libraryFilename,
  libraryBuild,
  libraryInstall,

  archive,
};
