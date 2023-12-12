import path from 'path';
import * as platform from './platform';
import {Configuration} from './configuration';
import {globSync} from 'glob';

const source = process.env['E2E_SOURCE'] || process.cwd();
const build = path.join(source, 'cmake-build');

const install = path.join(build, 'installation');
const include = path.join(install, 'include');
const lib = path.join(install, 'lib');
const cmake = path.join(lib, 'cmake');

const configFilename = 'FocusriteE2EConfig.cmake';
const cmakeConfigSource = path.join(source, 'cmake', configFilename);
const cmakeConfigDestination = path.join(cmake, configFilename);

const includeSource = path.join(source, 'source', 'cpp', 'include');

const library = (configuration: Configuration) => {
  const configDir = path.join(build, 'source', 'cpp', configuration);
  const extension = platform.isMac() ? 'a' : 'lib';
  const pattern = `*.${extension}`;

  const matches = globSync(pattern, {
    cwd: configDir,
  });

  return matches.length > 0 ? path.join(configDir, matches[0]) : undefined;
};

const archive = path.join(
  install,
  `focusrite-e2e-${platform.currentPlatform()}.tar.gz`
);

export default {
  source,
  build,
  install,
  include,
  lib,
  cmake,
  cmakeConfigSource,
  cmakeConfigDestination,
  includeSource,
  library,
  archive,
};
