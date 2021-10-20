import {EventEmitter} from 'events';
import Constants from './constants';

export enum ResponseType {
  response = 'response',
  event = 'event',
}

export interface Response {
  uuid: string;
  type: ResponseType;
  success?: string;
  error?: string;
  data?: any;
}

export interface CommandResponse extends Response {}

export type Event = any;

export interface EventResponse extends Response {
  name: string;
  data: any;
}

export class ResponseStream extends EventEmitter {
  data: Buffer;

  constructor() {
    super();
    this.data = Buffer.alloc(0);
  }

  checkForData() {
    if (this.data.length < Constants.HEADER_SIZE) {
      return;
    }

    const magic = this.data.readUInt32LE(Constants.MAGIC_OFFSET);

    if (magic != Constants.MAGIC) {
      this.emit('error', new Error('Incorrect magic number'));
      return;
    }

    const dataSize = this.data.readUInt32LE(Constants.SIZE_OFFSET);

    if (this.data.length < Constants.HEADER_SIZE + dataSize) {
      return;
    }

    const responseText = this.data
      .slice(Constants.DATA_OFFSET, Constants.DATA_OFFSET + dataSize)
      .toString();

    try {
      const response = JSON.parse(responseText) as Response;
      this.emit('response', response);
    } catch (error) {
      this.emit('error', new Error('Error parsing JSON response'));
    }

    this.data = this.data.slice(Constants.HEADER_SIZE + dataSize);

    if (this.data.length) {
      this.checkForData();
    }
  }

  push(data: Uint8Array) {
    this.data = Buffer.concat([this.data, data]);
    this.checkForData();
  }
}
