const currentPlatform = () =>
  process.platform === 'darwin' ? 'macos' : 'windows';

const isMac = () => currentPlatform() === 'macos';

module.exports = {
  currentPlatform,
  isMac,
};
