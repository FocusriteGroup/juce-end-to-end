import {AppConnection} from '../../source/ts';
import {ComponentHandle} from '../../source/ts/component-handle';
import {appPath} from './app-path';

describe('Accessibility tests', () => {
  let appConnection: AppConnection;
  let valueLabel: ComponentHandle;
  let incrementButton: ComponentHandle;
  let decrementButton: ComponentHandle;
  let enableButton: ComponentHandle;
  let slider: ComponentHandle;
  let window: ComponentHandle;

  beforeEach(async () => {
    appConnection = new AppConnection({appPath});
    valueLabel = appConnection.getComponent('value-label');
    incrementButton = appConnection.getComponent('increment-button');
    decrementButton = appConnection.getComponent('decrement-button');
    enableButton = appConnection.getComponent('enable-button');
    slider = appConnection.getComponent('slider');
    window = appConnection.getComponent('main-window');

    await appConnection.launch();
    await valueLabel.waitToBeVisible();
  });

  afterEach(async () => {
    await appConnection.quit();
  });

  it('Increment button is accessible', async () => {
    expect(incrementButton.getAccessibilityState()).resolves.toEqual({
      title: 'Increment button title',
      description: 'Increment button description',
      help: 'Increment button tool tip',
      accessible: true,
      handler: true,
      display: '',
    });
  });

  it('Decrement button is not accessible', async () => {
    const state = await decrementButton.getAccessibilityState();
    expect(state.accessible).toBeFalsy();
    expect(state.handler).toBeFalsy();
  });

  it('Enable button is enabled for accessibility but has no text', async () => {
    const state = await enableButton.getAccessibilityState();
    expect(state.accessible).toBeTruthy();
    expect(state.handler).toBeTruthy();
  });

  it('Slider is enabled for accessibility but has no text', async () => {
    const state = await slider.getAccessibilityState();
    expect(state.accessible).toBeTruthy();
    expect(state.handler).toBeTruthy();
    expect(parseFloat(state.display)).toEqual(0);
  });

  it('Slider display changes with the value change', async () => {
    expect(parseFloat((await slider.getAccessibilityState()).display)).toEqual(
      0
    );

    await incrementButton.click();
    expect(parseFloat((await slider.getAccessibilityState()).display)).toEqual(
      1
    );

    await decrementButton.click();
    expect(parseFloat((await slider.getAccessibilityState()).display)).toEqual(
      0
    );
  });

  it('Increment button has accessibility parent', async () => {
    expect(incrementButton.getAccessibilityParent()).resolves.toEqual(
      window.componentID
    );
    expect(enableButton.getAccessibilityParent()).resolves.toEqual(
      window.componentID
    );
    expect(slider.getAccessibilityParent()).resolves.toEqual(
      window.componentID
    );
  });

  it('Decrement button has no accessibility parent', async () => {
    expect(decrementButton.getAccessibilityParent()).resolves.toEqual('');
  });

  it('Main window has multiple accessible children', async () => {
    const children = await window.getAccessibilityChildren();
    expect(children.length).toBeGreaterThan(0);
    expect(children).toContainEqual(incrementButton.componentID);
    expect(children).toContainEqual(enableButton.componentID);
    expect(children).toContainEqual(slider.componentID);
    expect(children.includes(decrementButton.componentID)).toBeFalsy();
  });
});
