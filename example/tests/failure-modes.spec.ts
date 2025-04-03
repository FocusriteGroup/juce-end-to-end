import {AppConnection} from '../../source/ts';
import {ComponentHandle} from '../../source/ts/component-handle';
import {appPath} from './app-path';

describe('invalid component', () => {
  let app: AppConnection;

  beforeEach(async () => {
    app = new AppConnection({appPath, logDirectory: 'logs'});
    await app.launch();
  });

  afterEach(async () => {
    await app.quit();
  });

  it.failing('fails when waiting for invalid components', async () => {
    await app.getComponent('invalid').waitToBeVisible(100);
  });

  it.failing('fails when waiting for an event that never happens', async () => {
    await app.waitForEvent('my-event', undefined, 100);
  });
});

it.failing('rejects requests after the app has quit', async () => {
  const app = new AppConnection({appPath, logDirectory: 'logs'});
  await app.launch();
  await app.quit();
  await app.getComponent('value-label').waitToBeVisible(100);
});

it.failing('fails when the app crashes', async () => {
  const app = new AppConnection({appPath, logDirectory: 'logs'});
  await app.launch();
  app.sendCommand({type: 'abort'});
  await app.waitForExit();
});
