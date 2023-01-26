const DEFAULT_TIMEOUT = 10000;

export async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout = DEFAULT_TIMEOUT
): Promise<boolean> {
  return new Promise((resolve) => {
    const timeoutTimer = setTimeout(() => {
      resolve(false);
    }, timeout);

    const query = () => {
      queryFunction().then((data) => {
        if (matchingFunction(data)) {
          clearTimeout(timeoutTimer);
          resolve(true);
          return;
        }

        query();
      });
    };

    query();
  });
}

export async function waitForResult<T>(
  queryFunction: () => Promise<T>,
  expectedResult: T,
  timeoutMs = DEFAULT_TIMEOUT
): Promise<void> {
  const result = await pollUntil(
    (currentResult) => currentResult === expectedResult,
    queryFunction,
    timeoutMs
  );

  if (!result) {
    throw new Error(`Result didn't become ${expectedResult}`);
  }
}
