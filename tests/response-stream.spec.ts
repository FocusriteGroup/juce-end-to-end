import {responseToBuffer} from '../source/binary-protocol';
import {v4 as uuidv4} from 'uuid';
import {ResponseType} from '../source/responses';
import {ResponseStream} from '../source/response-stream';

describe('Response Stream', () => {
  let responseStream: ResponseStream;
  let onResponse: jest.Mock;
  let onError: jest.Mock;

  beforeEach(() => {
    responseStream = new ResponseStream();
    onResponse = jest.fn();
    onError = jest.fn();
    responseStream.on('response', onResponse);
    responseStream.on('error', onError);
  });

  afterEach(() => {
    responseStream = null;
  });

  it('parses valid response', () => {
    responseStream.push(responseToBuffer(exampleResponse));
    expect(onResponse).toHaveBeenCalledWith(exampleResponse);
  });

  it('parses valid response arriving byte by byte', () => {
    responseToBuffer(exampleResponse).forEach((byte) =>
      responseStream.push(Buffer.from([byte]))
    );
    expect(onResponse).toHaveBeenCalledWith(exampleResponse);
  });

  it('rejects invalid data', () => {
    responseStream.push(Buffer.from([1, 2, 3, 4, 5, 6, 7, 8]));
    expect(onError).toHaveBeenCalled();
  });
});

const exampleResponse = {
  uuid: uuidv4(),
  type: ResponseType.response,
  foo: 'foo',
  bar: 'bar',
};
