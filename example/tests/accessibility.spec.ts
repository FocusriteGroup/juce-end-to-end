import {AppConnection} from '../../source/ts';
import {appPath} from './app-path';

describe('Accessibility tests', () => {
  let appConnection: AppConnection;

  beforeEach(async () => {
    appConnection = new AppConnection({appPath});
    await appConnection.launch();
    await appConnection.waitForComponentToBeVisible('value-label');
  });

  afterEach(async () => {
    await appConnection.quit();
  });

  it('Increment button is accessible', async () => {
    expect(
      await appConnection.getAccessibilityState('increment-button')
    ).toEqual({
      title: 'Increment button title',
      description: 'Increment button description',
      help: 'Increment button tool tip',
      accessible: true,
      handler: true,
      display: '',
    });
  });

  it('Decrement button is not accessible', async () => {
    const state = await appConnection.getAccessibilityState('decrement-button');
    expect(state.accessible).toBeFalsy();
    expect(state.handler).toBeFalsy();
  });

  it('Enable button is enabled for accessibility but has no text', async () => {
    const state = await appConnection.getAccessibilityState('enable-button');
    expect(state.accessible).toBeTruthy();
    expect(state.handler).toBeTruthy();
  });

  it('Slider is enabled for accessibility but has no text', async () => {
    const state = await appConnection.getAccessibilityState('slider');
    expect(state.accessible).toBeTruthy();
    expect(state.handler).toBeTruthy();
    expect(parseFloat(state.display)).toEqual(0);
  });

  it('Slider display changes with the value change', async () => {
    expect(
      parseFloat((await appConnection.getAccessibilityState('slider')).display)
    ).toEqual(0);

    await appConnection.clickComponent('increment-button');
    expect(
      parseFloat((await appConnection.getAccessibilityState('slider')).display)
    ).toEqual(1);

    await appConnection.clickComponent('decrement-button');
    expect(
      parseFloat((await appConnection.getAccessibilityState('slider')).display)
    ).toEqual(0);
  });

  it('Increment button has accessibility parent', async () => {
    expect(
      await appConnection.getAccessibilityParent('increment-button')
    ).toEqual('main-window');
    expect(await appConnection.getAccessibilityParent('enable-button')).toEqual(
      'main-window'
    );
    expect(await appConnection.getAccessibilityParent('slider')).toEqual(
      'main-window'
    );
  });

  it('Decrement button has no accessibility parent', async () => {
    expect(
      await appConnection.getAccessibilityParent('decrement-button')
    ).toEqual('');
  });

  it('Main window has multiple accessible children', async () => {
    const children = await appConnection.getAccessibilityChildren(
      'main-window'
    );
    expect(children.length).toBeGreaterThan(0);
    expect(children).toContainEqual('increment-button');
    expect(children).toContainEqual('enable-button');
    expect(children).toContainEqual('slider');
  });

  it('Decrement is not a child of the main window', async () => {
    const children = await appConnection.getAccessibilityChildren(
      'main-window'
    );
    expect(children.find((str) => str === 'disable-button')).toBeFalsy();
  });
});
