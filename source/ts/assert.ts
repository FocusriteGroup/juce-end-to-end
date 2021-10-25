export const assert = (condition: boolean, message?: string) => {
  if (!condition) {
    console.error('Error');

    if (message) {
      console.error(message);
    }

    process.exit(1);
  }
};
