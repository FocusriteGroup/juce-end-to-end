import {EventEmitter} from 'events';
import path from 'path';
import {spawn, ChildProcess} from 'child_process';
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
  ResponseData,
} from './responses';
import {Command} from './commands';
import minimatch from 'minimatch';
import {strict as assert} from 'assert';
import {pollUntil} from './poll';

const writeFile = util.promisify(fs.writeFile);

let screenshotIndex = 0;

interface AppConnectionOptions {
  appPath: string;
  logDirectory?: string;
}

export interface EnvironmentVariables {
  [key: string]: string;
}

const DEFAULT_TIMEOUT = 5000;

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

  launchProcess(extraArgs: string[], env: EnvironmentVariables = {}) {
    try {
      this.process = spawn(this.appPath, extraArgs, {env});
    } catch (error) {
      console.error(`Unable to launch: ${error.message}`);
      return;
    }

    this.process.on('exit', (code, signal) => {
      this.emit('exit', {code, signal});
    });
  }

  async launch(extraArgs: string[] = [], env: EnvironmentVariables = {}) {
    const port = await this.server.listen();
    this.launchProcess(extraArgs.concat([`--e2e-test-port=${port}`]), env);
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

    if (this.process) {
      this.process.kill();
    }
  }

  async sendCommand(command: Command): Promise<ResponseData> {
    assert(this.connection);
    return await this.connection.send(command);
  }

  async waitForEvent(
    name: string,
    matchingFunction?: EventMatchingFunction,
    timeout?: number
  ): Promise<void> {
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

  async clickComponent(componentId: string, skip?: number): Promise<void> {
    await this.sendCommand({
      type: 'click-component',
      args: {
        'component-id': componentId,
        'num-clicks': 1,
        'skip': skip || 0,
      },
    });
  }

  async doubleClickComponent(
    componentId: string,
    skip?: number
  ): Promise<void> {
    await this.sendCommand({
      type: 'click-component',
      args: {
        'component-id': componentId,
        'num-clicks': 2,
        'skip': skip || 0,
      },
    });
  }

  async grabFocus(needNullFocus: boolean): Promise<void> {
    await this.sendCommand({
      type: 'grab-focus',
      args: {
        'require-null-focus': needNullFocus,
      },
    });
  }

  async keyPress(
    key: string,
    modifiers?: string,
    focusComponent?: string
  ): Promise<void> {
    await this.sendCommand({
      type: 'key-press',
      args: {
        'key-code': key,
        'modifiers': modifiers || '',
        'focus-component': focusComponent || '',
      },
    });
  }

  async invokeMenu(menu: string): Promise<void> {
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
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<boolean> {
    const result = await pollUntil(
      (visible: boolean) => visible === visibility,
      async () => await this.getComponentVisibility(componentName),
      timeoutInMilliseconds
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

  async waitForComponentToBeVisible(
    componentName: string,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<boolean> {
    return await this.waitForComponentVisibilityToBe(
      componentName,
      true,
      timeoutInMilliseconds
    );
  }

  async waitForComponentEnablementToBe(
    componentName: string,
    enablement: boolean,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<boolean> {
    const result = await pollUntil(
      (enabled: boolean) => enabled === enablement,
      async () => await this.getComponentEnablement(componentName),
      timeoutInMilliseconds
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

  async waitForComponentToBeEnabled(
    componentName: string,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<boolean> {
    return await this.waitForComponentEnablementToBe(
      componentName,
      true,
      timeoutInMilliseconds
    );
  }

  async waitForComponentToBeDisabled(
    componentName: string,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<boolean> {
    return await this.waitForComponentEnablementToBe(
      componentName,
      false,
      timeoutInMilliseconds
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

  async saveScreenshot(
    componentId: string,
    outFileName: string
  ): Promise<void> {
    if (!this.logDirectory) {
      console.error(
        'Unable to save a screenshot of the app as a log directory has not been set'
      );
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

    return response.showing;
  }

  async getComponentEnablement(componentId: string): Promise<boolean> {
    const response = (await this.sendCommand({
      type: 'get-component-enablement',
      args: {
        'component-id': componentId,
      },
    })) as ComponentEnablementResponse;

    return response.enabled;
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

  async tabToComponent(
    componentPattern: string,
    maxNumComponents = 1024
  ): Promise<boolean> {
    for (let count = 0; count < maxNumComponents; count++) {
      await this.keyPress('tab');
      const focusedId = await this.getFocusedComponent();
      if (focusedId && minimatch(focusedId, componentPattern)) {
        return true;
      }
    }

    return false;
  }
}
