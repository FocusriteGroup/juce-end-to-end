const DEFAULT_TIMEOUT = 10000;

const wait = async (milliseconds: number) =>
  new Promise((resolve) => setTimeout(resolve, milliseconds));

export async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout = DEFAULT_TIMEOUT
): Promise<boolean> {
  const end = Date.now() + timeout;

  do {
    const data = await queryFunction();

    if (matchingFunction(data)) {
      return true;
    }

    const pollInterval = 5;
    await wait(pollInterval);
  } while (Date.now() < end);

  return false;
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
