import {AppConnection} from '.';
import {AccessibilityResponse} from './responses';
import {DEFAULT_TIMEOUT} from './app-connection';

export class ComponentHandle {
  appConnection: AppConnection;
  componentID: string;

  constructor(componentID: string, appConnection: AppConnection) {
    this.appConnection = appConnection;
    this.componentID = componentID;
  }

  async waitToBeVisible(timeoutInMilliseconds?: number) {
    await this.appConnection.waitForComponentToBeVisible(
      this.componentID,
      timeoutInMilliseconds
    );
  }

  async waitForVisibilityToBe(
    visibility: boolean,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ) {
    await this.appConnection.waitForComponentVisibilityToBe(
      this.componentID,
      visibility,
      timeoutInMilliseconds
    );
  }

  async isVisible(): Promise<boolean> {
    return this.appConnection.getComponentVisibility(this.componentID);
  }

  async waitToBeEnabled(timeoutInMilliseconds = DEFAULT_TIMEOUT) {
    await this.appConnection.waitForComponentToBeEnabled(
      this.componentID,
      timeoutInMilliseconds
    );
  }

  async waitToBeDisabled(timeoutInMilliseconds = DEFAULT_TIMEOUT) {
    await this.appConnection.waitForComponentToBeDisabled(
      this.componentID,
      timeoutInMilliseconds
    );
  }

  async waitForEnablementToBe(
    enablement: boolean,
    timeoutInMilliseconds = DEFAULT_TIMEOUT
  ) {
    await this.appConnection.waitForComponentEnablementToBe(
      this.componentID,
      enablement,
      timeoutInMilliseconds
    );
  }

  async isEnabled(): Promise<boolean> {
    return this.appConnection.getComponentEnablement(this.componentID);
  }

  async getText(): Promise<string> {
    return this.appConnection.getComponentText(this.componentID);
  }

  async setTextEditorText(text: string) {
    await this.appConnection.setTextEditorText(this.componentID, text);
  }

  async getEnablement(): Promise<boolean> {
    return this.appConnection.getComponentEnablement(this.componentID);
  }

  async click(skip?: number) {
    await this.appConnection.clickComponent(this.componentID, skip);
  }

  async doubleClick(skip?: number) {
    await this.appConnection.doubleClickComponent(this.componentID, skip);
  }

  async getSliderValue(): Promise<number> {
    return this.appConnection.getSliderValue(this.componentID);
  }

  async setSliderValue(value: number) {
    await this.appConnection.setSliderValue(this.componentID, value);
  }

  async getComboBoxSelectedItemIndex(): Promise<number> {
    return this.appConnection.getComboBoxSelectedItemIndex(this.componentID);
  }

  async setComboBoxSelectedItemIndex(index: number) {
    await this.appConnection.setComboBoxSelectedItemIndex(
      this.componentID,
      index
    );
  }

  async getComboBoxItems(): Promise<string[]> {
    return this.appConnection.getComboBoxItems(this.componentID);
  }

  async getComboBoxNumItems(): Promise<number> {
    return this.appConnection.getComboBoxNumItems(this.componentID);
  }

  async getAccessibilityState(): Promise<AccessibilityResponse> {
    return this.appConnection.getAccessibilityState(this.componentID);
  }

  async getAccessibilityParent(): Promise<string> {
    return this.appConnection.getAccessibilityParent(this.componentID);
  }

  async getAccessibilityChildren(): Promise<Array<string>> {
    return this.appConnection.getAccessibilityChildren(this.componentID);
  }

  async keyPress(key: string, modifiers?: string) {
    await this.appConnection.keyPress(key, modifiers, this.componentID);
  }

  async isFocused(): Promise<boolean> {
    return (
      (await this.appConnection.getFocusedComponent()) === this.componentID
    );
  }

  async tabTo(maxNumComponents = 1024): Promise<boolean> {
    return this.appConnection.tabToComponent(
      this.componentID,
      maxNumComponents
    );
  }

  async getNumChildrenWithID(childID: string): Promise<number> {
    return this.appConnection.countComponents(childID, this.componentID);
  }

  async saveScreenshot(outFileName: string) {
    await this.appConnection.saveScreenshot(this.componentID, outFileName);
  }
}
