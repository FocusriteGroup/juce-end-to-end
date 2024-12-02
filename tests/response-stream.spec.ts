import {toBuffer} from '../source/ts/binary-protocol';
import {v4 as uuidv4} from 'uuid';
import {ResponseType} from '../source/ts/responses';
import {ResponseStream} from '../source/ts/response-stream';

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

  it('parses valid response', () => {
    responseStream.push(toBuffer(exampleResponse));
    expect(onResponse).toHaveBeenCalledWith(exampleResponse);
  });

  it('parses valid response arriving byte by byte', () => {
    toBuffer(exampleResponse).forEach((byte) =>
      responseStream.push(Buffer.from([byte]))
    );
    expect(onResponse).toHaveBeenCalledWith(exampleResponse);
  });

  it('rejects invalid data', () => {
    responseStream.push(Buffer.from([1, 2, 3, 4, 5, 6, 7, 8]));
    expect(onError).toHaveBeenCalled();
  });

  it('rejects invalid data with a valid header', () => {
    const buffer = toBuffer(exampleResponse);
    buffer[buffer.length - 1] = '∆'.charCodeAt(0);

    responseStream.push(buffer);
    expect(onError).toHaveBeenCalled();
  });

  it('retrieves valid responses after receiving an invalid response', () => {
    const invalidBuffer = toBuffer(exampleResponse);
    invalidBuffer[invalidBuffer.length - 1] = '∆'.charCodeAt(0);

    const validBuffer1 = toBuffer(exampleResponse);

    const exampleResponse2 = {hello: 'world'};
    const validBuffer2 = toBuffer(exampleResponse2);

    responseStream.push(
      Buffer.concat([invalidBuffer, validBuffer1, validBuffer2])
    );

    expect(onError).toHaveBeenCalled();
    expect(onResponse).toHaveBeenNthCalledWith(1, exampleResponse);
    expect(onResponse).toHaveBeenNthCalledWith(2, exampleResponse2);
  });

  it('parses valid responses arriving together', () => {
    const response1 = {hello: 'world'};
    const response2 = {foo: 'bar'};

    const combinedBuffer = Buffer.concat([
      toBuffer(response1),
      toBuffer(response2),
    ]);

    responseStream.push(combinedBuffer);

    expect(onResponse).toHaveBeenCalledTimes(2);
    expect(onResponse).toHaveBeenNthCalledWith(1, response1);
    expect(onResponse).toHaveBeenNthCalledWith(2, response2);
  });

  it('parses valid responses arriving in irregular chunks', () => {
    const response1 = {hello: 'world'};
    const response2 = {foo: 'bar'};

    const combinedBuffer = Buffer.concat([
      toBuffer(response1),
      toBuffer(response2),
    ]);

    const bufferSize = combinedBuffer.length;
    const chunkSize = Math.floor(bufferSize / 3);

    responseStream.push(combinedBuffer.subarray(0, chunkSize));
    responseStream.push(combinedBuffer.subarray(chunkSize, 2 * chunkSize));
    responseStream.push(combinedBuffer.subarray(2 * chunkSize));

    expect(onResponse).toHaveBeenCalledTimes(2);
    expect(onResponse).toHaveBeenNthCalledWith(1, response1);
    expect(onResponse).toHaveBeenNthCalledWith(2, response2);
  });
});

const exampleResponse = {
  uuid: uuidv4(),
  type: ResponseType.response,
  foo: 'foo',
  bar: 'bar',
};
