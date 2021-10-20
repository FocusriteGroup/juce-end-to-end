export interface ComponentCountResponse extends Response {
  count: number;
}

export interface ScreenshotResponse extends Response {
  image: string;
}

export interface ComponentVisibilityResponse extends Response {
  showing: boolean;
  exists: boolean;
}

export interface ComponentEnablementResponse extends Response {
  enabled: boolean;
  exists: boolean;
}

export interface ComponentTextResponse extends Response {
  text: string;
}

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
