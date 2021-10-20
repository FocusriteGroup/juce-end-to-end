export interface ComponentCountResponse {
  count: number;
}

export interface ScreenshotResponse {
  image: string;
}

export interface ComponentVisibilityResponse {
  showing: string;
}

export interface ComponentEnablementResponse {
  enabled: string;
}

export interface ComponentTextResponse {
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
