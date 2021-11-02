import net, {Socket} from 'net';
import {strict as assert} from 'assert';
import {EventEmitter} from 'events';

export class Server extends EventEmitter {
  server?: net.Server;
  lastConnection?: net.Socket;

  constructor() {
    super();
  }

  close() {
    if (this.server) {
      this.server.close();
      this.server = null;
    }
  }

  async listen(): Promise<number> {
    return new Promise((resolve) => {
      assert(!this.server);

      this.server = new net.Server();

      this.server.on('close', () => {
        this.emit('close');
        this.server = null;
      });

      this.server.on('connection', (socket) => {
        this.lastConnection = socket;
        this.emit('connection', socket);
      });

      this.server.on('error', (error) => {
        console.error(`Error on listen socket: ${error.message}`);

        if (this.server) {
          this.server.close();
        }

        this.emit('error', error);
      });

      this.server.on('listening', () => {
        const address = this.server.address() as net.AddressInfo;
        resolve(address.port);
      });

      this.server.listen();
    });
  }

  async waitForConnection(): Promise<Socket> {
    return new Promise((resolve) => {
      if (this.lastConnection) {
        resolve(this.lastConnection);
        this.lastConnection = null;
      } else {
        assert(!!this.server);
        this.server.on('connection', (socket) => {
          resolve(socket);
        });
      }
    });
  }
}
