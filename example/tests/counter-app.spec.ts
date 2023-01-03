import {AppConnection} from '../../source/ts';
import {appPath} from './app-path';

describe('Count App tests', () => {
  let appConnection: AppConnection;

  beforeEach(async () => {
    appConnection = new AppConnection({appPath});
    await appConnection.launch();
    await appConnection.waitForComponentToBeVisible('value-label');
  });

  afterEach(async () => {
    await appConnection.quit();
  });

  it('starts at 0', async () => {
    const value = await appConnection.getComponentText('value-label');
    expect(value).toEqual('0');
  });

  it('increments using the increment button', async () => {
    await appConnection.clickComponent('increment-button');

    const value = await appConnection.getComponentText('value-label');
    expect(value).toEqual('1');
  });

  it('decrements using the decrement button', async () => {
    await appConnection.clickComponent('decrement-button');

    const value = await appConnection.getComponentText('value-label');
    expect(value).toEqual('-1');
  });

  it('can be disabled', async () => {
    expect(
      await appConnection.getComponentEnablement('increment-button')
    ).toBeTruthy();
    expect(
      await appConnection.getComponentEnablement('decrement-button')
    ).toBeTruthy();

    await appConnection.clickComponent('enable-button');

    expect(
      await appConnection.getComponentEnablement('increment-button')
    ).toBeFalsy();
    expect(
      await appConnection.getComponentEnablement('decrement-button')
    ).toBeFalsy();
  });

  it('sets value using the slider', async () => {
    const expectedValue = 6;
    await appConnection.setSliderValue('slider', expectedValue);
    const value = await appConnection.getSliderValue('slider');
    expect(value).toBe(expectedValue);
  });

  it('sets value using the text editor', async () => {
    const expectedValue = 789;
    await appConnection.setTextEditorText('text-editor', `${expectedValue}`);
    const value = await appConnection.getComponentText('value-label');
    expect(value).toBe(`${expectedValue}`);
  });
});
