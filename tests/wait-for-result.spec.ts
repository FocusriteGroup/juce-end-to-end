import {waitForResult} from '../source/ts/poll';

describe('waiting for a function to return a particular value', () => {
  const functionThatReturns42 = () => Promise.resolve(42);
  const timeoutMs = 10;

  it('becomes the expected value within the timeout time', async () => {
    const expectedValue = 42;

    await waitForResult(functionThatReturns42, expectedValue, timeoutMs);
  });

  it('throws when the function does not return the expected value within the timeout time', async () => {
    const expectedValue = 13;

    expect(
      waitForResult(functionThatReturns42, expectedValue, timeoutMs)
    ).rejects.toThrow();
  });
});
