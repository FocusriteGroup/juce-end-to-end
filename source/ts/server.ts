import net, {Socket} from 'net';
import {EventEmitter} from 'events';

export class Server extends EventEmitter {
  listenSocket?: net.Server;
  lastConnection?: net.Socket;

  constructor() {
    super();
  }

  close() {
    if (this.listenSocket) {
      this.listenSocket.close();
    }
  }

  async listen(): Promise<number> {
    return new Promise((resolve) => {
      if (this.listenSocket) {
        throw new Error('Server already running');
      }

      this.listenSocket = new net.Server();

      this.listenSocket.on('close', () => {
        this.emit('close');
        this.listenSocket = undefined;
      });

      this.listenSocket.on('connection', (socket) => {
        this.lastConnection = socket;
        this.emit('connection', socket);
      });

      this.listenSocket.on('error', (error) => {
        console.error(`Error on listen socket: ${error.message}`);

        if (this.listenSocket) {
          this.listenSocket.close();
        }

        this.emit('error', error);
      });

      this.listenSocket.on('listening', () => {
        if (!this.listenSocket) {
          throw new Error('Listen socket has closed');
        }

        const address = this.listenSocket.address() as net.AddressInfo;
        resolve(address.port);
      });

      this.listenSocket.listen();
    });
  }

  async waitForConnection(): Promise<Socket> {
    if (this.lastConnection) {
      const connection = this.lastConnection;
      this.lastConnection = undefined;
      return Promise.resolve(connection);
    }

    return new Promise((resolve, reject) => {
      if (!this.listenSocket) {
        reject(new Error('Listen socket has closed'));
        return;
      }

      this.listenSocket.on('connection', resolve);
    });
  }
}
