import {Command} from './commands';
import {AppConnection} from './app-connection';

interface E2EEventHandlers {}

interface E2EOptions {
  appPath: string;
  eventHandlers?: E2EEventHandlers;
  appArguments?: string[];
}

export class E2EController {
  appConnection: AppConnection;
  eventHandlers: E2EEventHandlers;
  appArguments?: string[];

  constructor(options: E2EOptions) {
    this.appConnection = new AppConnection({appPath: options.appPath});
    this.eventHandlers = options.eventHandlers;
    this.appArguments = options.appArguments;
  }

  async start() {
    await this.appConnection.launch(this.appArguments);
  }

  async sendCommand(command: Command) {
    await this.appConnection.sendCommand(command);
  }

  async quit(): Promise<void> {
    return new Promise((resolve, reject) => {
      this.appConnection.on('exit', (info) => {
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

      this.appConnection.quit();
    });
  }
}
