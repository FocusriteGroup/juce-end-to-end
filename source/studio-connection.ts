import {EventEmitter} from 'events';
import path from 'path';
import {spawn, execSync, ChildProcess} from 'child_process';
import process from 'process';
import assert from 'assert';
import util from 'util';
import fs from 'fs';
import {Connection, EventMatchingFunction} from './connection';
import {Server} from './server';
import {
  ComponentVisibilityResponse,
  ComponentEnablementResponse,
  ComponentTextResponse,
  LoginStateResponse,
  ModelResponse,
  ScreenshotResponse,
  StateResponse,
  ComponentCountResponse,
} from './responses';
import {Command} from './commands';
import minimatch from 'minimatch';

const writeFile = util.promisify(fs.writeFile);

const VERBOSE = process.env['VERBOSE'] || false;

function getBuildType() {
  return process.argv.includes('--debug') ? 'Debug' : 'Release';
}

function getBuiltProductPath() {
  return path.join(__dirname, '..', '..', '..', 'cmake-build', getBuildType());
}

function getDarwinExecutablePath() {
  return path.join(
    getBuiltProductPath(),
    'Ampify Studio.app',
    'Contents',
    'MacOS',
    'Ampify Studio'
  );
}

function getWindowsExecutablePath() {
  return path.join(getBuiltProductPath(), 'Ampify Studio.exe');
}

function getDefaultPath() {
  if (process.platform === 'darwin') {
    return getDarwinExecutablePath();
  }
  return getWindowsExecutablePath();
}

const logsDir = path.join(__dirname, '..', '..', '..', 'logs');

let screenshotIndex = 0;

export enum ExportType {
  project = 'project',
  section = 'section',
}

export enum ExportFormat {
  wav = 'wav',
  mp3 = 'mp3',
}

export class StudioConnection extends EventEmitter {
  appPath: string;
  process?: ChildProcess;
  server: Server;
  connection?: Connection;

  constructor(appPath?: string) {
    super();
    this.appPath = appPath || process.env['AMPIFY_STUDIO'] || getDefaultPath();
    this.process = null;
    this.server = new Server();

    this.server.on('error', () => {
      this.stopServer();
    });

    this.server.on('close', () => {
      this.server = null;
    });

    if (!fs.existsSync(logsDir)) {
      fs.mkdirSync(logsDir);
    }
  }

  stopServer() {
    this.server.close();
    this.connection.kill();
  }

  connect() {
    assert(this.process);
  }

  launchProcess(extraArgs: string[]) {
    let args = [
      '--bypass-updater',
      '--bring-to-front',
      '--verbose-logging',
      '--disable-analytics',
      `--log-dir=${logsDir}`,
      '--no-startup-project',
      '--no-user-reload',
      '--no-quick-start',
    ];

    if (typeof extraArgs === 'object') {
      args = args.concat(extraArgs);
    } else if (typeof extraArgs === 'string') {
      args.push(extraArgs);
    }

    const studioEnv = process.env.FORCE_ENABLE_AUDIO
      ? {
          env: {REPORT_AUDIO_AS_ENABLED: 'true'},
        }
      : {};

    try {
      this.process = spawn(this.appPath, args, studioEnv);
    } catch (error) {
      console.error(`Unable to launch: ${error.message}`);
      return;
    }

    this.process.on('exit', (code, signal) => {
      if (signal) {
        console.log(`Studio exited with signal ${signal}`);
      }

      this.emit('exit', {code, signal});
    });

    if (VERBOSE) {
      this.process.stdout.on('data', (data) => {
        data
          .toString()
          .split('\n')
          .forEach((line: string) => {
            console.log(`>> ${line}`);
          });
      });

      this.process.stderr.on('data', (data) => {
        data
          .toString()
          .split('\n')
          .forEach((line: string) => {
            console.log(`>> ${line}`);
          });
      });
    }
  }

  async launch(extraArgs: string[] = []) {
    const port = await this.server.listen();
    this.launchProcess(extraArgs.concat([`--e2e-test-port=${port}`]));
    const socket = await this.server.waitForConnection();

    this.connection = new Connection(socket);
    this.connection.on('connect', () => this.emit('connect'));
    this.connection.on('disconnect', () => {
      this.emit('disconnect');
      this.connection.kill();
      this.server.close();
    });
  }

  kill() {
    this.connection.kill();
    this.server.close();
    if (this.process) this.process.kill();
  }

  async sendCommand(command: Command): Promise<any> {
    assert(this.connection);
    return await this.connection.send(command);
  }

  async waitForEvent(
    name: string,
    matchingFunction?: EventMatchingFunction,
    timeout?: number
  ) {
    await this.connection.waitForEvent(name, matchingFunction, timeout);
  }

  async quit() {
    await this.sendCommand({
      type: 'quit',
    });
  }

  async importFile(filePath: string) {
    await this.sendCommand({
      type: 'import-file',
      args: {
        path: filePath,
      },
    });
  }

  async clickComponent(componentId: string, skip?: number) {
    await this.sendCommand({
      type: 'click-component',
      args: {
        'component-id': componentId,
        'num-clicks': 1,
        'skip': skip || 0,
      },
    });
  }

  async doubleClickComponent(componentId: string) {
    await this.sendCommand({
      type: 'click-component',
      args: {
        'component-id': componentId,
        'num-clicks': 2,
      },
    });
  }

  async grabFocus(needNullFocus: boolean) {
    await this.sendCommand({
      type: 'grab-focus',
      args: {
        'require-null-focus': needNullFocus,
      },
    });
  }

  async keyPress(key: string, modifiers?: string, focusComponent?: string) {
    // First, make sure that the app is focused
    if (process.platform === 'darwin') {
      let appPath = this.appPath.substring(0, this.appPath.indexOf('.app'));
      appPath += '.app';
      execSync(`open '${appPath}'`);
    }

    await this.sendCommand({
      type: 'key-press',
      args: {
        'key-code': key,
        'modifiers': modifiers || '',
        'focus-component': focusComponent || '',
      },
    });
  }

  async invokeMenu(menu: string) {
    await this.sendCommand({
      type: 'invoke-menu',
      args: {
        title: menu,
      },
    });
  }

  async openProject(projectPath: string) {
    await this.sendCommand({
      type: 'open-project',
      args: {
        path: projectPath,
      },
    });

    await this.waitForEvent('project-loaded', (event) => {
      return event.path === projectPath;
    });
  }

  async closeProject() {
    await this.sendCommand({
      type: 'close-project',
      args: {},
    });
  }

  async getModel() {
    const response = (await this.sendCommand({
      type: 'get-model',
      args: {},
    })) as ModelResponse;

    try {
      return JSON.parse(response.model);
    } catch (error) {
      console.log(`Error parsing model: ${error.message}`);
    }
    return {};
  }

  async specQuery<T>(
    matchingFunction: (data: T) => boolean,
    queryFunction: () => Promise<T>,
    timeout?: number
  ) {
    if (!timeout) timeout = 10000;

    return new Promise((resolve) => {
      const timeoutTimer = setTimeout(() => {
        resolve(false);
      }, timeout);

      const query = () => {
        queryFunction().then((data) => {
          if (matchingFunction(data)) {
            clearTimeout(timeoutTimer);
            resolve(true);
            return;
          }

          query();
        });
      };

      query();
    });
  }

  async modelMeetsSpec(
    matchingFunction: (model: any) => boolean,
    timeout?: number
  ) {
    return this.specQuery(
      matchingFunction,
      () => {
        return this.getModel();
      },
      timeout
    );
  }

  async stateMeetsSpec(
    matchingFunction: (state: any) => boolean,
    timeout?: number
  ) {
    return this.specQuery(
      matchingFunction,
      () => {
        return this.getState();
      },
      timeout
    );
  }

  async waitForComponentVisibilityToBe(
    componentName: string,
    visibility: boolean,
    timeoutTime = 5000
  ) {
    const result = await this.specQuery(
      (visible: any) => visible == visibility,
      async () => await this.getComponentVisibility(componentName),
      timeoutTime
    );

    if (!result) {
      const errorDescription = visibility ? 'visible' : 'hidden';
      const filename = `${++screenshotIndex}.png`;
      console.error(
        `Component '${componentName}' didn't become ${errorDescription}, writing screenshot to ${filename}`
      );
      await this.getScreenshot('', filename);
      throw new Error(
        `Component '${componentName}' didn't become ${errorDescription}`
      );
    }

    return result;
  }

  async waitForComponentToBeVisible(componentName: string, timeoutTime = 5000) {
    return await this.waitForComponentVisibilityToBe(
      componentName,
      true,
      timeoutTime
    );
  }

  async waitForComponentEnablementToBe(
    componentName: string,
    enablement: boolean,
    timeoutTime = 5000
  ) {
    const result = await this.specQuery(
      (enabled: boolean) => enabled === enablement,
      async () => await this.getComponentEnablement(componentName),
      timeoutTime
    );

    const stateString = enablement ? 'enabled' : 'disabled';
    const failString = `Component '${componentName}' didn't become ${stateString}`;

    if (!result) {
      const filename = `${++screenshotIndex}.png`;
      console.error(`${failString}, writing screenshot to ${filename}`);
      await this.getScreenshot('', filename);
      throw new Error(failString);
    }

    return result;
  }

  async waitForComponentToBeEnabled(componentName: string, timeoutTime = 5000) {
    return await this.waitForComponentEnablementToBe(
      componentName,
      true,
      timeoutTime
    );
  }

  async waitForComponentToBeDisabled(
    componentName: string,
    timeoutTime = 5000
  ) {
    return await this.waitForComponentEnablementToBe(
      componentName,
      false,
      timeoutTime
    );
  }

  async countComponents(
    componentId: string,
    rootId: string
  ): Promise<ComponentCountResponse> {
    const result = await this.sendCommand({
      type: 'get-component-count',
      args: {
        'component-id': componentId,
        'root-id': rootId,
      },
    });
    result.count = Number(result.count);
    return result;
  }

  async newProject() {
    await this.sendCommand({
      type: 'new-project',
      args: {},
    });
  }

  async getScreenshot(componentId: string, outFileName: string) {
    const response = (await this.sendCommand({
      type: 'get-screenshot',
      args: {
        'component-id': componentId,
      },
    })) as ScreenshotResponse;

    try {
      await writeFile(
        path.join(logsDir, outFileName),
        response.image,
        'base64'
      );
      console.log(`Screenshot of ${componentId} written to ${outFileName}`);
    } catch (error) {
      console.error(
        `Error writing screenshot of ${componentId} to ${outFileName}`
      );
    }
  }

  async getState() {
    const response = (await this.sendCommand({
      type: 'get-state',
      args: {},
    })) as StateResponse;

    try {
      return JSON.parse(response.state);
    } catch (error) {
      console.error(`Error parsing state: ${error.message}`);
    }

    return {};
  }

  async getComponentVisibility(componentId: string) {
    const response = (await this.sendCommand({
      type: 'get-component-visibility',
      args: {
        'component-id': componentId,
      },
    })) as ComponentVisibilityResponse;

    return response.showing && response.showing === 'true';
  }

  async getComponentEnablement(componentId: string) {
    const response = (await this.sendCommand({
      type: 'get-component-enablement',
      args: {
        'component-id': componentId,
      },
    })) as ComponentEnablementResponse;

    return response.enabled && response.enabled === 'true';
  }

  async getComponentText(componentId: string) {
    const response = (await this.sendCommand({
      type: 'get-component-text',
      args: {
        'component-id': componentId,
      },
    })) as ComponentTextResponse;

    return response.text;
  }

  async getUserLoginState() {
    const response = (await this.sendCommand({
      type: 'get-user-login-state',
      args: {},
    })) as LoginStateResponse;
    return response.loginState && response.loginState === 'true';
  }

  async getPreviewState() {
    const response = await this.getState();
    return (
      response.previewingPack &&
      response.previewingPack !== '00000000-0000-0000-0000-000000000000'
    );
  }

  async waitForUserLoginStateToBe(state: boolean, timeoutTime = 5000) {
    const result = await this.specQuery(
      (isLoggedIn: boolean) => isLoggedIn === state,
      () => this.getUserLoginState(),
      timeoutTime
    );

    if (!result) {
      console.error(`Login state didn't go to '${state}' in time`);
    }

    return result;
  }

  async login(username: string, password: string) {
    await this.sendCommand({
      type: 'login',
      args: {
        username: username,
        password: password,
      },
    });
  }

  async logout() {
    await this.sendCommand({
      type: 'logout',
      args: {},
    });
  }

  async waitForPreviewStateToBe(previewState: boolean, timeoutTime = 500000) {
    const result = await this.specQuery(
      (isPreviewing: boolean) => isPreviewing === previewState,
      () => this.getPreviewState(),
      timeoutTime
    );

    if (!result) {
      console.error(`Preview state didn't go to '${previewState}' in time`);
    }

    return result;
  }

  async liveExport(projectDirectory: string) {
    await this.sendCommand({
      type: 'live-export',
      args: {path: projectDirectory},
    });
  }

  async audioExport(
    outputPath: string,
    type: ExportType,
    format: ExportFormat,
    sectionIndex?: number
  ) {
    await this.sendCommand({
      type: 'audio-export',
      args: {
        path: outputPath,
        type,
        format,
        sectionIndex,
      },
    });
  }

  async getFocusedComponent() {
    const response = await this.sendCommand({
      type: 'get-focus-component',
      args: {},
    });
    if (response && response['component-id']) {
      return response['component-id'];
    }
    return null;
  }

  async tabToComponent(componentId: string) {
    for (let count = 0; count < 1024; count++) {
      await this.keyPress('tab');
      const focusedId = await this.getFocusedComponent();
      if (focusedId && minimatch(focusedId, componentId)) {
        return true;
      }
    }

    return false;
  }

  async enableExperimentalFeatures() {
    await this.sendCommand({
      type: 'enable-experimental-features',
      args: {},
    });
  }

  async useRemoteConfig(config: object) {
    await this.sendCommand({
      type: 'remote-config',
      args: {
        config,
      },
    });
  }
}
