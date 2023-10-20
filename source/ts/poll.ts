const DEFAULT_TIMEOUT = 10000;

const wait = async (milliseconds: number) =>
  new Promise((resolve) => setTimeout(resolve, milliseconds));

const rejectAfter = (timeoutMs: number) =>
  new Promise<never>((_, reject) =>
    setTimeout(() => reject(new Error('Timed out')), timeoutMs)
  );

export async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout = DEFAULT_TIMEOUT
): Promise<boolean> {
  const end = Date.now() + timeout;

  while (Date.now() < end) {
    try {
      const data = await Promise.race([
        queryFunction(),
        rejectAfter(end - Date.now()),
      ]);

      if (matchingFunction(data)) {
        return true;
      }
    } catch {
      return false;
    }

    const pollInterval = 5;
    await wait(pollInterval);
  }

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
