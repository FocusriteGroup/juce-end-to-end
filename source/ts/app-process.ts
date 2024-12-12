import {spawn} from 'child_process';
import path from 'path';
import fs from 'fs';

export interface EnvironmentVariables {
  [key: string]: string;
}

const nextRunId = (() => {
  let runId = 1;

  return () => {
    return runId++;
  };
})();

const createLogFiles = (logDirectory: string) => {
  const runId = nextRunId();

  const stdoutPath = path.join(
    logDirectory,
    `application-tests-stdout-${runId}.log`
  );
  const stderrPath = path.join(
    logDirectory,
    `application-tests-stderr-${runId}.log`
  );

  return {
    stdout: fs.createWriteStream(stdoutPath, {flags: 'a'}),
    stderr: fs.createWriteStream(stderrPath, {flags: 'a'}),
  };
};

interface Options {
  path: string;
  logDirectory?: string;
  extraArgs: string[];
  env: EnvironmentVariables;
}

export const launchApp = ({path, extraArgs, env, logDirectory}: Options) => {
  const process = spawn(path, extraArgs, {env});

  if (logDirectory) {
    const logs = createLogFiles(logDirectory);
    process.stdout.pipe(logs.stdout);
    process.stderr.pipe(logs.stderr);
  }

  return process;
};

export type AppProcess = ReturnType<typeof launchApp>;
