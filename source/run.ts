import {AppConnection} from './index';

const appPath =
  '/Users/joenoel/dev/juce-end-to-end/cmake-build-debug/example/app/e2e-example-app_artefacts/Debug/e2e-example-app.app/Contents/MacOS/e2e-example-app';

async function run() {
  const connection = new AppConnection({
    appPath,
  });

  await connection.launch();
  await connection.waitForComponentToBeVisible('value-label');
  const textBefore = await connection.getComponentText('value-label');
  console.log(textBefore);
  await connection.waitForComponentToBeVisible('increment-button');
  await connection.clickComponent('increment-button');
  const textAfter = await connection.getComponentText('value-label');
  console.log(textAfter);
  await connection.quit();

  console.log('Done');
}

run();
