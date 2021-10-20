import {EventEmitter} from 'events';
import {
  ResponseStream,
  Response,
  CommandResponse,
  EventResponse,
  Event,
  ResponseType,
} from './response-stream';
import Constants from './constants';
import {v4 as uuidv4} from 'uuid';
import {assert} from './assert';
import {Socket} from 'net';
import {Command} from './commands';

export type EventMatchingFunction = (event: Event) => boolean;

interface WaitingEvent {
  name: string;
  matchingFunction?: EventMatchingFunction;
  onReceived(event: object): void;
  onError(): void;
}

interface SentCommand extends Command {
  uuid: string;
  onReceived(response: Buffer): void;
  onError(error: Error): void;
}

function commandToBuffer(command: SentCommand) {
  const commandJson = JSON.stringify(command);
  const commandBuffer = Buffer.from(commandJson, 'utf-8');

  const buffer = Buffer.alloc(Constants.HEADER_SIZE + commandBuffer.length, 0);
  buffer.writeUInt32LE(Constants.MAGIC, Constants.MAGIC_OFFSET);
  buffer.writeUInt32LE(commandBuffer.length, Constants.SIZE_OFFSET);
  commandBuffer.copy(buffer, Constants.DATA_OFFSET);

  return buffer;
}

export class Connection extends EventEmitter {
  responseStream: ResponseStream;
  sentCommands: SentCommand[];
  receivedEvents: EventResponse[];
  waitingEvents: WaitingEvent[];
  socket: Socket;

  constructor(socket: Socket) {
    super();
    this.responseStream = new ResponseStream();
    this.sentCommands = [];
    this.receivedEvents = [];
    this.waitingEvents = [];
    this.socket = socket;

    this.socket.on('close', () => {
      this.emit('disconnect');
    });

    this.socket.on('connect', () => {
      this.emit('connect');
    });

    this.socket.on('data', (data) => {
      this.responseStream.push(data);
    });

    this.responseStream.on('response', (response: Response) => {
      if (response.type === ResponseType.response) {
        this.responseReceived(response as CommandResponse);
      } else if (response.type === ResponseType.event) {
        this.eventReceived(response as EventResponse);
      }
    });

    this.responseStream.on('error', (error) => {
      console.error(
        `Response error from ${Constants.APP_NAME}: ${error.message}`
      );
      this.socket.destroy();
    });

    this.socket.on('error', (error) => {
      if (error.message !== 'read ECONNRESET') {
        console.log(
          `SOCKET ERROR: Response error from ${Constants.APP_NAME}: ${error.message}`
        );
      }

      this.socket.destroy();
    });
  }

  kill() {
    if (this.socket) this.socket.destroy();
  }

  async send(command: Command): Promise<object> {
    return new Promise((resolve, reject) => {
      assert(!!this.socket, 'Not connected');
      const sentCommand = {
        uuid: uuidv4(),
        ...command,
        onReceived: (data: any) => resolve(data),
        onError: (error: Error) => reject(error),
      };
      const buffer = commandToBuffer(sentCommand);
      this.socket.write(buffer);
      this.sentCommands.push(sentCommand);
    });
  }

  async waitForEvent(
    name: string,
    matchingFunction?: (event: object) => boolean,
    timeout?: number
  ) {
    return new Promise((resolve, reject) => {
      if (timeout) {
        setTimeout(reject, timeout);
      }
      this.waitingEvents.push({
        name,
        matchingFunction,
        onReceived: resolve,
        onError: reject,
      });
      this.notifyWaitingEvents();
    });
  }

  responseReceived(response: CommandResponse) {
    const command = this.sentCommands.find((element) => {
      return element.uuid === response.uuid;
    });

    if (!command) {
      return;
    }

    if (command && response.success) {
      command.onReceived(response.data);
    }

    if (!response.success && command) {
      command.onError(new Error(response.error));
    }

    this.sentCommands.splice(this.sentCommands.indexOf(command), 1);
  }

  eventReceived(event: EventResponse) {
    if (event.name) {
      this.receivedEvents.push(event);
      this.notifyWaitingEvents();
    }
  }

  clearEvents() {
    this.receivedEvents = [];
  }

  notifyWaitingEvents() {
    this.waitingEvents = this.waitingEvents.filter((waitingEvent) => {
      let removeElement = false;

      this.receivedEvents.forEach((receivedEvent) => {
        if (waitingEvent.name !== receivedEvent.name) {
          return;
        }

        if (
          waitingEvent.matchingFunction &&
          !waitingEvent.matchingFunction(receivedEvent.data)
        ) {
          return;
        }

        if (!waitingEvent.onReceived) {
          return;
        }

        waitingEvent.onReceived(receivedEvent.data);
        removeElement = true;
      });

      return !removeElement;
    });
  }
}
