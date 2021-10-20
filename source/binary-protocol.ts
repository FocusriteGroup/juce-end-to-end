import {SentCommand} from './commands';
import {Response} from './responses';

const constants = {
  HEADER_SIZE: 8,
  DATA_OFFSET: 8,
  MAGIC_OFFSET: 0,
  SIZE_OFFSET: 4,
  MAGIC: 0x30061990,
};

export function commandToBuffer(command: SentCommand) {
  const commandJson = JSON.stringify(command);
  const commandBuffer = Buffer.from(commandJson, 'utf-8');

  const buffer = Buffer.alloc(constants.HEADER_SIZE + commandBuffer.length, 0);
  buffer.writeUInt32LE(constants.MAGIC, constants.MAGIC_OFFSET);
  buffer.writeUInt32LE(commandBuffer.length, constants.SIZE_OFFSET);
  commandBuffer.copy(buffer, constants.DATA_OFFSET);

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
  const dataSize = buffer.readUInt32LE(constants.SIZE_OFFSET);

  if (buffer.length < constants.HEADER_SIZE + dataSize) {
    return {bytesConsumed: 0};
  }

  const rawResponse = buffer
    .slice(constants.DATA_OFFSET, constants.DATA_OFFSET + dataSize)
    .toString();

  return {
    response: JSON.parse(rawResponse) as Response,
    bytesConsumed: constants.HEADER_SIZE + dataSize,
  };
}
