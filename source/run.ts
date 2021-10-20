import {E2EController} from './index';

const appPath =
  '/Users/joenoel/dev/juce-end-to-end/cmake-build-debug/example/app/e2e-example-app_artefacts/Debug/e2e-example-app.app/Contents/MacOS/e2e-example-app';

async function run() {
  const controller = new E2EController({
    appPath,
  });

  await controller.start();

  await controller.appConnection.waitForComponentToBeVisible('value-label');
  const textBefore = await controller.appConnection.getComponentText(
    'value-label'
  );
  console.log(textBefore);
  await controller.appConnection.waitForComponentToBeVisible(
    'increment-button'
  );
  await controller.appConnection.clickComponent('increment-button');
  const textAfter = await controller.appConnection.getComponentText(
    'value-label'
  );
  console.log(textAfter);
  await controller.quit();

  console.log('Done');
}

run();
