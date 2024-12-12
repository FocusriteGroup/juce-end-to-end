export interface Command {
  type: string;
  args?: object;
}

export interface SentCommand extends Command {
  uuid: string;
  onReceived(response?: object): void;
  onError(error: Error): void;
}
