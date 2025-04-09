import {AppConnection} from '../../source/ts';
import {appPath} from './app-path';
import {ComponentHandle} from '../../source/ts/component-handle';

describe('Count App tests', () => {
  let appConnection: AppConnection;
  let valueLabel: ComponentHandle;
  let incrementButton: ComponentHandle;
  let decrementButton: ComponentHandle;
  let enableButton: ComponentHandle;
  let slider: ComponentHandle;
  let textEditor: ComponentHandle;
  let comboBox: ComponentHandle;

  beforeEach(async () => {
    appConnection = new AppConnection({appPath});
    valueLabel = appConnection.getComponent('value-label');
    incrementButton = appConnection.getComponent('increment-button');
    decrementButton = appConnection.getComponent('decrement-button');
    enableButton = appConnection.getComponent('enable-button');
    slider = appConnection.getComponent('slider');
    textEditor = appConnection.getComponent('text-editor');
    comboBox = appConnection.getComponent('combo-box');

    await appConnection.launch();
    await valueLabel.waitToBeVisible();
  });

  afterEach(async () => {
    await appConnection.quit();
  });

  it('starts at 0', async () => {
    expect(valueLabel.getText()).resolves.toEqual('0');
  });

  it('increments using the increment button', async () => {
    await incrementButton.click();
    expect(valueLabel.getText()).resolves.toEqual('1');
  });

  it('decrements using the decrement button', async () => {
    await decrementButton.click();
    expect(valueLabel.getText()).resolves.toEqual('-1');
  });

  it('can be disabled', async () => {
    expect(incrementButton.getEnablement()).resolves.toBeTruthy();
    expect(decrementButton.getEnablement()).resolves.toBeTruthy();

    await enableButton.click();

    expect(incrementButton.getEnablement()).resolves.toBeFalsy();
    expect(decrementButton.getEnablement()).resolves.toBeFalsy();
  });

  it('sets value using the slider', async () => {
    expect(slider.getSliderValue()).resolves.toBe(0);

    const expectedValue = 6;
    slider.setSliderValue(expectedValue);
    expect(slider.getSliderValue()).resolves.toBe(expectedValue);
  });

  it('sets value using the text editor', async () => {
    const expectedValue = '789';
    await textEditor.setTextEditorText(expectedValue);
    expect(valueLabel.getText()).resolves.toBe(expectedValue);
  });

  it('sets value using the combo-box', async () => {
    const expectedValue = 2;
    await comboBox.setComboBoxSelectedItemIndex(expectedValue);
    expect(comboBox.getComboBoxSelectedItemIndex()).resolves.toBe(
      expectedValue
    );
  });

  it('checks that all values in combox box are present and in the correct order', async () => {
    expect(comboBox.getComboBoxItems()).resolves.toStrictEqual([
      'First',
      'Second',
      'Third',
    ]);
  });
});
