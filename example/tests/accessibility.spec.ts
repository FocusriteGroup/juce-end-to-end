import exp from 'constants';
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
    expect((await appConnection.getAccessibilityState('increment-button'))).toEqual({
        "title": 'Increment button title',
        "description": 'Increment button description',
        "help": 'Increment button help text',
        "accessible": true,
        "handler": true,
    });
  });

  it('Decrement button is not accessible', async () => {
    expect((await appConnection.getAccessibilityState('decrement-button'))).toEqual({
        "title": '',
        "description": '',
        "help": '',
        "accessible": false,
        "handler": false,
    });
  });

  it('Enable button is enabled for accessibility but has no text', async () => {
    expect((await appConnection.getAccessibilityState('enable-button'))).toEqual({
        "title": '',
        "description": '',
        "help": '',
        "accessible": true,
        "handler": true,
    });
  });

  it('Slider is enabled for accessibility but has no text', async () => {
    expect((await appConnection.getAccessibilityState('slider'))).toEqual({
        "title": '',
        "description": '',
        "help": '',
        "accessible": true,
        "handler": true,
    });
  });
});
