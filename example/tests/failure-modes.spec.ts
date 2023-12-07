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

  it('rejects invalid components', async () => {
    await expect(
      app.waitForComponentToBeVisible('invalid', 100)
    ).rejects.toThrow();
  });
});

it('rejects requests after the app has quit', async () => {
  const app = new AppConnection({appPath, logDirectory: 'logs'});
  await app.launch();
  await app.quit();
  await expect(
    app.waitForComponentToBeVisible('value-label')
  ).rejects.toThrow();
});
