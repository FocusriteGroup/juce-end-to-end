import {pollUntil} from '../source/ts/poll';

const wait = async (milliseconds: number) =>
  new Promise((resolve) => setTimeout(resolve, milliseconds));

describe('using pollUntil to see if a function matches a value', () => {
  const functionThatReturns42 = () => Promise.resolve(42);
  const timeoutMs = 10;

  it('resolves if the expected value is returned within the timeout time', async () => {
    const didReturnExpectedValue = pollUntil(
      (currentResult) => currentResult === 42,
      functionThatReturns42,
      timeoutMs
    );

    await expect(didReturnExpectedValue).resolves.not.toThrow();
  });

  it('rejects if the expected value is not returned within the timeout time', async () => {
    const didReturnExpectedValue = pollUntil(
      (currentResult) => currentResult === 21,
      functionThatReturns42,
      timeoutMs
    );
    await expect(didReturnExpectedValue).rejects.toThrow();
  });

  it('rejects if the query function never returns anything within the timeout time', async () => {
    const longerThanTheTimeout = timeoutMs * 2;

    const didReturnExpectedValue = pollUntil(
      (currentResult) => currentResult === 42,
      async () => {
        await wait(longerThanTheTimeout);
        return await functionThatReturns42();
      },
      timeoutMs
    );

    await expect(didReturnExpectedValue).rejects.toThrow();
  });
});
