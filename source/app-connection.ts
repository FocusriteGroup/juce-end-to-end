import {EventEmitter} from 'events';
import path from 'path';
import {spawn, execSync, ChildProcess} from 'child_process';
import process from 'process';
import util from 'util';
import fs from 'fs';
import {Connection, EventMatchingFunction} from './connection';
import {Server} from './server';
import {
  ComponentVisibilityResponse,
  ComponentEnablementResponse,
  ComponentTextResponse,
  ScreenshotResponse,
  ComponentCountResponse,
} from './responses';
import {Command} from './commands';
import minimatch from 'minimatch';
import {assert} from './assert';
import {Response} from '.';

const writeFile = util.promisify(fs.writeFile);

let screenshotIndex = 0;

interface AppConnectionOptions {
  appPath: string;
  logDirectory?: string;
}

export class AppConnection extends EventEmitter {
  appPath: string;
  process?: ChildProcess;
  server: Server;
  connection?: Connection;
  logDirectory?: string;

  constructor(options: AppConnectionOptions) {
    super();
    this.appPath = options.appPath;
    this.logDirectory = options.logDirectory;
    this.server = new Server();

    this.server.on('error', () => {
      this.stopServer();
    });

    this.server.on('close', () => {
      this.server = null;
    });

    if (this.logDirectory && !fs.existsSync(this.logDirectory)) {
      fs.mkdirSync(this.logDirectory);
    }
  }

  stopServer() {
    this.server.close();
    this.connection.kill();
  }

  connect() {
    assert(!!this.process);
  }

  launchProcess(extraArgs: string[]) {
    try {
      let command = this.appPath;
      if (command.endsWith('.app')) {
        command = 'open';
        extraArgs = ['-a', this.appPath, '--'].concat(extraArgs);
      }
      console.log('command');
      console.log(command);
      console.log('extra args');
      console.log(extraArgs);
      this.process = spawn(command, extraArgs, {});
    } catch (error) {
      console.error(`Unable to launch: ${error.message}`);
      return;
    }

    this.process.on('exit', (code, signal) => {
      this.emit('exit', {code, signal});
    });
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

  async sendCommand(command: Command): Promise<Response> {
    assert(!!this.connection);
    return await this.connection.send(command);
  }

  async waitForEvent(
    name: string,
    matchingFunction?: EventMatchingFunction,
    timeout?: number
  ) {
    await this.connection.waitForEvent(name, matchingFunction, timeout);
  }

  async waitForExit(): Promise<void> {
    return new Promise((resolve, reject) => {
      this.on('exit', (info) => {
        if (info.code !== 0) {
          reject(`App exited with code ${info.code}`);
          return;
        }

        if (info.signal) {
          reject(`App exited with signal ${info.signal}`);
          return;
        }

        resolve();
      });
    });
  }

  async quit(): Promise<void> {
    const promise = this.waitForExit();
    this.sendCommand({
      type: 'quit',
    });
    return promise;
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

  async waitForComponentVisibilityToBe(
    componentName: string,
    visibility: boolean,
    timeoutTime = 5000
  ) {
    const result = await pollUntil(
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
      await this.saveScreenshot('', filename);
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
    const result = await pollUntil(
      (enabled: boolean) => enabled === enablement,
      async () => await this.getComponentEnablement(componentName),
      timeoutTime
    );

    const stateString = enablement ? 'enabled' : 'disabled';
    const failString = `Component '${componentName}' didn't become ${stateString}`;

    if (!result) {
      const filename = `${++screenshotIndex}.png`;
      console.error(`${failString}, writing screenshot to ${filename}`);
      await this.saveScreenshot('', filename);
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

  async countComponents(componentId: string, rootId: string): Promise<number> {
    const result = (await this.sendCommand({
      type: 'get-component-count',
      args: {
        'component-id': componentId,
        'root-id': rootId,
      },
    })) as ComponentCountResponse;

    return result.count;
  }

  async saveScreenshot(componentId: string, outFileName: string) {
    if (!this.logDirectory) {
      return;
    }

    const response = (await this.sendCommand({
      type: 'get-screenshot',
      args: {
        'component-id': componentId,
      },
    })) as ScreenshotResponse;

    try {
      await writeFile(
        path.join(this.logDirectory, outFileName),
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

  async getComponentVisibility(componentId: string): Promise<boolean> {
    const response = (await this.sendCommand({
      type: 'get-component-visibility',
      args: {
        'component-id': componentId,
      },
    })) as ComponentVisibilityResponse;

    return response.showing && response.showing === 'true';
  }

  async getComponentEnablement(componentId: string): Promise<boolean> {
    const response = (await this.sendCommand({
      type: 'get-component-enablement',
      args: {
        'component-id': componentId,
      },
    })) as ComponentEnablementResponse;

    return response.enabled && response.enabled === 'true';
  }

  async getComponentText(componentId: string): Promise<string> {
    const response = (await this.sendCommand({
      type: 'get-component-text',
      args: {
        'component-id': componentId,
      },
    })) as ComponentTextResponse;

    return response.text;
  }

  async getFocusedComponent(): Promise<string | undefined> {
    const response = await this.sendCommand({
      type: 'get-focus-component',
      args: {},
    });

    return response && response['component-id'];
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
}

async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout?: number
) {
  if (!timeout) {
    timeout = 10000;
  }

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
