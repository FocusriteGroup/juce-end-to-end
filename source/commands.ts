export interface Command {
  type: string;
  args?: any;
}

export interface SentCommand extends Command {
  uuid: string;
  onReceived(response: Buffer): void;
  onError(error: Error): void;
}
