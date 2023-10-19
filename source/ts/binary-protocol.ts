import {Response} from './responses';

const constants = {
  HEADER_SIZE: 8,
  DATA_OFFSET: 8,
  MAGIC_OFFSET: 0,
  SIZE_OFFSET: 4,
  MAGIC: 0x30061990,
};

export function toBuffer(data: object) {
  const dataBuffer = Buffer.from(JSON.stringify(data), 'utf-8');

  const buffer = Buffer.alloc(constants.HEADER_SIZE + dataBuffer.length, 0);
  buffer.writeUInt32LE(constants.MAGIC, constants.MAGIC_OFFSET);
  buffer.writeUInt32LE(dataBuffer.length, constants.SIZE_OFFSET);
  dataBuffer.copy(buffer, constants.DATA_OFFSET);

  return buffer;
}

export function isValid(buffer: Buffer): boolean {
  if (buffer.length < constants.HEADER_SIZE) {
    return true;
  }

  return buffer.readUInt32LE(constants.MAGIC_OFFSET) == constants.MAGIC;
}

interface NextResponse {
  response?: Response;
  bytesConsumed: number;
}

export function getNextResponse(buffer: Buffer): NextResponse {
  if (buffer.length < constants.HEADER_SIZE) {
    return {bytesConsumed: 0};
  }

  const dataSize = buffer.readUInt32LE(constants.SIZE_OFFSET);

  if (buffer.length < constants.HEADER_SIZE + dataSize) {
    return {bytesConsumed: 0};
  }

  const rawResponse = buffer
    .subarray(constants.DATA_OFFSET, constants.DATA_OFFSET + dataSize)
    .toString();

  return {
    response: JSON.parse(rawResponse) as Response,
    bytesConsumed: constants.HEADER_SIZE + dataSize,
  };
}
