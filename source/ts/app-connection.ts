import {EventEmitter} from 'events';
import path from 'path';
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
  GetSliderValueResponse,
  GetItemIndexResponse,
  AccessibilityResponse,
  AccessibilityParentResponse,
  AccessibilityChildResponse,
  GetFocusedComponentResponse,
} from './responses';
import {Command} from './commands';
import minimatch from 'minimatch';
import {waitForResult} from './poll';
import {AppProcess, EnvironmentVariables, launchApp} from './app-process';

const writeFile = util.promisify(fs.writeFile);

let screenshotIndex = 0;

interface AppConnectionOptions {
  appPath?: string;
  logDirectory?: string;
  listenPort?: number;
}

const DEFAULT_TIMEOUT = 5000;

const existsAsFile = (path: string) => {
  try {
    return fs.statSync(path).isFile();
  } catch (error) {
    return false;
  }
};

export class AppConnection extends EventEmitter {
  appPath?: string;
  process?: AppProcess;
  server: Server;
  connection?: Connection;
  logDirectory?: string;
  listenPort?: number;
  exitPromise?: Promise<void>;

  constructor(options: AppConnectionOptions) {
    super();

    this.appPath = options.appPath;
    this.logDirectory = options.logDirectory;
    this.listenPort = options.listenPort;
    this.server = new Server();

    this.server.on('error', () => {
      this.stopServer();
    });

    if (this.logDirectory) {
      fs.mkdirSync(this.logDirectory, {
        recursive: true,
      });
    }
  }

  stopServer() {
    this.server.close();
    this.connection?.kill();
  }

  async waitForClient() {
    if (!this.listenPort) {
      throw new Error(
        `Listening port must be specific to wait for client`
      );
    }

    await this.server.listen(this.listenPort);
    const socket = await this.server.waitForConnection();

    this.connection = new Connection(socket);
    this.connection.on('connect', () => this.emit('connect'));
    this.connection.on('disconnect', () => {
      this.server.close();
      this.connection = undefined;
      this.emit('disconnect');
    });
  }

  launchProcess(extraArgs: string[], env: EnvironmentVariables = {}) {
    if (!this.appPath) {
      throw new Error(
        `App path must be specified to launch process`
      );
    }
    if (!existsAsFile(this.appPath)) {
      throw new Error(
        `The specified app path (${this.appPath}) doesn't exist`
      );
    }

    this.process = launchApp({
      path: this.appPath,
      logDirectory: this.logDirectory,
      extraArgs,
      env,
    });

    this.process.on('error', (error) => {
      throw new Error(`Failed to spawn process: ${error.message}`);
    });

    this.exitPromise = new Promise<void>((resolve, reject) => {
      this.process?.on('exit', (code, signal) => {
        this.stopServer();

        if (code) {
          reject(new Error(`App exited with error code: ${code}`));
        } else if (signal) {
          reject(new Error(`App exited with signal: ${signal}`));
        } else {
          resolve();
        }
      });
    });
  }

  async launch(extraArgs: string[] = [], env: EnvironmentVariables = {}) {
    const port = await this.server.listen(this.listenPort);
    this.launchProcess(extraArgs.concat([`--e2e-test-port=${port}`]), env);
    const socket = await this.server.waitForConnection();

    this.connection = new Connection(socket);
    this.connection.on('connect', () => this.emit('connect'));
    this.connection.on('disconnect', () => {
      this.server.close();
      this.connection = undefined;
      this.emit('disconnect');
    });
  }

  kill() {
    this.connection?.kill();
    this.server.close();
    this.process?.kill();
  }

  async sendCommand(command: Command): Promise<ResponseData> {
    if (!this.connection) {
      throw new Error('Not connected to application');
    }
    return await this.connection.send(command);
  }

  async waitForEvent(
    name: string,
    matchingFunction?: EventMatchingFunction,
    timeout?: number
  ): Promise<void> {
    if (!this.connection) {
      throw new Error('Not connected to application');
    }

    await this.connection.waitForEvent(name, matchingFunction, timeout);
  }

  async waitForExit(): Promise<void> {
    if (!this.exitPromise) {
      throw new Error(`Process hasn't been launched`);
    }

    await this.exitPromise;
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

  async grabFocus(windowId?: string): Promise<void> {
    await this.sendCommand({
      type: 'grab-focus',
      args: {
        'window-id': windowId || '',
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

  async setSliderValue(sliderId: string, value: number): Promise<void> {
    await this.sendCommand({
      type: 'set-slider-value',
      args: {
        'component-id': sliderId,
        value,
      },
    });
  }

  async getSliderValue(sliderId: string): Promise<number> {
    const response = (await this.sendCommand({
      type: 'get-slider-value',
      args: {
        'component-id': sliderId,
      },
    })) as GetSliderValueResponse;

    return response.value;
  }

  async getAccessibilityState(
    componentId: string
  ): Promise<AccessibilityResponse> {
    return (await this.sendCommand({
      type: 'get-accessibility-state',
      args: {
        'component-id': componentId,
      },
    })) as AccessibilityResponse;
  }

  async getAccessibilityParent(componentId: string): Promise<string> {
    const parentResponse = (await this.sendCommand({
      type: 'get-accessibility-parent',
      args: {
        'component-id': componentId,
      },
    })) as AccessibilityParentResponse;

    return parentResponse.parent;
  }

  async getAccessibilityChildren(componentId: string): Promise<Array<string>> {
    const childResponse = (await this.sendCommand({
      type: 'get-accessibility-children',
      args: {
        'component-id': componentId,
      },
    })) as AccessibilityChildResponse;

    return childResponse.children.filter((str) => str !== '');
  }

  async setTextEditorText(componentId: string, value: string): Promise<void> {
    await this.sendCommand({
      type: 'set-text-editor-text',
      args: {
        'component-id': componentId,
        value,
      },
    });
  }

  async setComboBoxSelectedItemIndex(
    comboBoxId: string,
    value: number
  ): Promise<void> {
    await this.sendCommand({
      type: 'set-combo-box-selected-item-index',
      args: {
        'component-id': comboBoxId,
        value,
      },
    });
  }

  async getComboBoxSelectedItemIndex(comboBoxId: string): Promise<number> {
    const response = (await this.sendCommand({
      type: 'get-combo-box-selected-item-index',
      args: {
        'component-id': comboBoxId,
      },
    })) as GetItemIndexResponse;

    return response.value;
  }

  async getComboBoxNumItems(comboBoxId: string): Promise<number> {
    const response = (await this.sendCommand({
      type: 'get-combo-box-num-items',
      args: {
        'component-id': comboBoxId,
      },
    })) as GetItemIndexResponse;

    return response.value;
  }

  async invokeMenu(menu: string, windowId?: string): Promise<void> {
    await this.sendCommand({
      type: 'invoke-menu',
      args: {
        'title': menu,
        'window-id': windowId || '',
      },
    });
  }

  async waitForComponentVisibilityToBe(
    componentName: string,
    visibility: boolean,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<void> {
    try {
      await waitForResult(
        () => this.getComponentVisibility(componentName),
        visibility,
        timeoutInMilliseconds
      );
    } catch (error) {
      const errorDescription = visibility ? 'visible' : 'hidden';
      const filename = `${++screenshotIndex}.png`;
      await this.saveScreenshot('', filename);
      throw new Error(
        `Component '${componentName}' didn't become ${errorDescription} (see screenshot ${filename})`
      );
    }
  }

  async waitForComponentToBeVisible(
    componentName: string,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ): Promise<void> {
    await this.waitForComponentVisibilityToBe(
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
    try {
      await waitForResult(
        () => this.getComponentEnablement(componentName),
        enablement,
        timeoutInMilliseconds
      );

      return true;
    } catch (error) {
      const stateString = enablement ? 'enabled' : 'disabled';
      const failString = `Component '${componentName}' didn't become ${stateString}`;

      const filename = `${++screenshotIndex}.png`;
      console.error(`${failString}, writing screenshot to ${filename}`);
      await this.saveScreenshot('', filename);
      throw new Error(failString);
    }
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
      const outputFile = path.join(this.logDirectory, outFileName);
      await writeFile(outputFile, response.image, 'base64');
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
    const response = (await this.sendCommand({
      type: 'get-focus-component',
      args: {},
    })) as GetFocusedComponentResponse;

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
