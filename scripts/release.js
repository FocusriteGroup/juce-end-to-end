const git = require('./lib/git');
const paths = require('./lib/paths');
const {GithubReleaser} = require('./lib/github');

const release = async () => {
  if (!git.isOnTag) {
    console.error('Not on tag');
    process.exit(1);
  }

  try {
    await new GithubReleaser({
      owner: 'Focusrite-Novation',
      repo: 'juce-end-to-end',
      tag: git.tag,
    }).release(paths.archive);
  } catch (error) {
    console.error(`Error releasing to GitHub: ${error}`);
    process.exit(1);
  }
};

release();
