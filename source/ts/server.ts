import net, {Socket} from 'net';
import {EventEmitter} from 'events';

export class Server extends EventEmitter {
  listenSocket?: net.Server;

  constructor() {
    super();
  }

  close() {
    if (this.listenSocket) {
      this.listenSocket.close();
    }
  }

  async listen(): Promise<number> {
    if (this.listenSocket) {
      throw new Error('Server already running');
    }

    this.listenSocket = new net.Server();

    this.listenSocket?.on('error', (error) => {
      this.listenSocket?.close();
      this.emit('error', error);
      throw new Error(`Error on listen socket: ${error.message}`);
    });

    this.listenSocket?.on('close', () => {
      this.listenSocket = undefined;
    });

    return new Promise((resolve) => {
      this.listenSocket?.on('listening', () => {
        if (!this.listenSocket) {
          throw new Error('Listen socket has closed');
        }

        const address = this.listenSocket.address() as net.AddressInfo;
        resolve(address.port);
      });

      this.listenSocket?.listen();
    });
  }

  async waitForConnection(): Promise<Socket> {
    if (!this.listenSocket) {
      throw new Error('Server not listening');
    }

    return new Promise<Socket>((resolve, reject) => {
      this.listenSocket?.on('close', () =>
        reject(new Error('Listen socket closed'))
      );

      this.listenSocket?.on('connection', resolve);
    });
  }
}
