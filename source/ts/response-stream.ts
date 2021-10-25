import {EventEmitter} from 'events';
import {getNextResponse, isValid} from './binary-protocol';

export class ResponseStream extends EventEmitter {
  data: Buffer;

  constructor() {
    super();
    this.data = Buffer.alloc(0);
  }

  #checkForData() {
    if (!isValid(this.data)) {
      this.emit('error', new Error('Incorrect response'));
      return;
    }

    try {
      const nextResponse = getNextResponse(this.data);

      if (nextResponse.bytesConsumed == 0) {
        return;
      }

      this.data = this.data.slice(nextResponse.bytesConsumed);

      if (!nextResponse.response) {
        return;
      }

      this.emit('response', nextResponse.response);
    } catch (error) {
      this.emit('error', new Error('Error parsing response'));
    }

    if (this.data.length) {
      this.#checkForData();
    }
  }

  push(data: Uint8Array) {
    this.data = Buffer.concat([this.data, data]);
    this.#checkForData();
  }
}
