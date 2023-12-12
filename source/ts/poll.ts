const DEFAULT_TIMEOUT = 10000;

const wait = async (milliseconds: number) =>
  new Promise((resolve) => setTimeout(resolve, milliseconds));

const invokeWithTimeout = async <T>(
  queryFunction: () => Promise<T>,
  timeoutMs: number
) => {
  let timer: NodeJS.Timeout | undefined = undefined;
  const rejectingPromise = new Promise<never>(
    (_, reject) =>
      (timer = setTimeout(() => reject(new Error('Timed out.')), timeoutMs))
  );

  try {
    return await Promise.race([queryFunction(), rejectingPromise]);
  } catch {
    throw new Error('Timed out.');
  } finally {
    clearTimeout(timer);
  }
};

export async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout = DEFAULT_TIMEOUT
): Promise<void> {
  const end = Date.now() + timeout;

  while (Date.now() < end) {
    try {
      const data = await invokeWithTimeout(queryFunction, end - Date.now());

      if (matchingFunction(data)) {
        return Promise.resolve();
      }
    } catch (error) {
      return Promise.reject(new Error('Timed out.'));
    }

    const pollInterval = 5;
    await wait(pollInterval);
  }

  return Promise.reject(new Error('Timed out.'));
}

export async function waitForResult<T>(
  queryFunction: () => Promise<T>,
  expectedResult: T,
  timeoutMs = DEFAULT_TIMEOUT
): Promise<void> {
  try {
    await pollUntil(
      (currentResult) => currentResult === expectedResult,
      queryFunction,
      timeoutMs
    );
  } catch {
    throw new Error(`Result didn't become ${expectedResult}`);
  }
}
