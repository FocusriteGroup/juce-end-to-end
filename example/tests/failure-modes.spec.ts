import {AppConnection} from '../../source/ts';
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
    await app.waitForComponentToBeVisible('invalid', 100);
  });

  it.failing('fails when waiting for an event that never happens', async () => {
    await app.waitForEvent('my-event', undefined, 100);
  });
});

it.failing('rejects requests after the app has quit', async () => {
  const app = new AppConnection({appPath, logDirectory: 'logs'});
  await app.launch();
  await app.quit();
  await app.waitForComponentToBeVisible('value-label');
});

it.failing('fails when the app crashes', async () => {
  const app = new AppConnection({appPath, logDirectory: 'logs'});
  await app.launch();
  app.sendCommand({type: 'abort'});
  await app.waitForExit();
});
