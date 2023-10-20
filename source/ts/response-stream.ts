import {EventEmitter} from 'events';
import {getNextResponse, isValid} from './binary-protocol';
import {strict as assert} from 'assert';

export class ResponseStream extends EventEmitter {
  data: Buffer;

  constructor() {
    super();
    this.data = Buffer.alloc(0);
  }

  #checkForData() {
    if (!isValid(this.data)) {
      this.emit('error', new Error('Invalid header'));
      return;
    }

    const nextResponse = getNextResponse(this.data);

    if (nextResponse.bytesConsumed === 0) {
      return;
    }

    this.data = this.data.subarray(nextResponse.bytesConsumed);

    assert(!!nextResponse.response);

    if (nextResponse.response instanceof Error) {
      this.emit('error', nextResponse.response);
      this.#checkForData();
      return;
    }

    this.emit('response', nextResponse.response);
    this.#checkForData();
  }

  push(data: Uint8Array) {
    this.data = Buffer.concat([this.data, data]);
    this.#checkForData();
  }
}
