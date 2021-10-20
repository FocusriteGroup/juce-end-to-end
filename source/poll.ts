export async function pollUntil<T>(
  matchingFunction: (data: T) => boolean,
  queryFunction: () => Promise<T>,
  timeout?: number
) {
  if (!timeout) {
    timeout = 10000;
  }

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
