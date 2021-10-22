export const currentPlatform = () =>
  process.platform === 'darwin' ? 'macos' : 'windows';

export const isMac = () => currentPlatform() === 'macos';
