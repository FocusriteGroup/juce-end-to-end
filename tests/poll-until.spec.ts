import {pollUntil} from '../source/ts/poll';

const wait = async (milliseconds: number) =>
  new Promise((resolve) => setTimeout(resolve, milliseconds));

describe('using pollUntil to see if a function matches a value', () => {
  const functionThatReturns42 = () => Promise.resolve(42);
  const timeoutMs = 10;

  it('returns true if the expected value is returned within the timeout time', async () => {
    const didReturnExpectedValue = await pollUntil(
      (currentResult) => currentResult === 42,
      () => functionThatReturns42(),
      timeoutMs
    );

    expect(didReturnExpectedValue).toBe(true);
  });

  it('returns false if the expected value is not returned within the timeout time', async () => {
    const didReturnExpectedValue = await pollUntil(
      (currentResult) => currentResult === 21,
      () => functionThatReturns42(),
      timeoutMs
    );

    expect(didReturnExpectedValue).toBe(false);
  });

  it('returns false if the query function never returns anything within the timeout time', async () => {
    const longerThanTheTimeout = timeoutMs * 2;

    const didReturnExpectedValue = await pollUntil(
      (currentResult) => currentResult === 42,
      async () => {
        await wait(longerThanTheTimeout);
        return functionThatReturns42();
      },
      timeoutMs
    );

    expect(didReturnExpectedValue).toBe(false);
  });
});
