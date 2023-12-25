export interface ComponentCountResponse {
  count: number;
}

export interface ScreenshotResponse {
  image: string;
}

export interface ComponentVisibilityResponse {
  showing: boolean;
  exists: boolean;
}

export interface ComponentEnablementResponse {
  enabled: boolean;
  exists: boolean;
}

export interface ComponentTextResponse {
  text: string;
}

export interface GetSliderValueResponse {
  value: number;
}

export interface GetItemIndexResponse {
  value: number;
}

export interface AccessibilityResponse {
  title: string;
  description: string;
  help: string;
  accessible: boolean;
  handler: boolean;
  display: string;
}

export interface AccessibilityParentResponse {
  parent: string;
}

export interface AccessibilityChildResponse {
  children: string[];
}

export interface GetFocusedComponentResponse {
  'component-id': string;
}

export interface ComponentTreeResponse {
  components: Object;
}

export enum ResponseType {
  response = 'response',
  event = 'event',
}

export type ResponseData = object;

export interface Response {
  uuid: string;
  type: ResponseType;
  success?: string;
  error?: string;
  data?: ResponseData;
}

export type Event = object;

export interface EventResponse extends Response {
  name: string;
  data: ResponseData;
}
