import git from './lib/git';
import paths from './lib/paths';
import {GithubReleaser} from './lib/github';

const release = async () => {
  if (!git.isOnTag) {
    console.error('Not on tag');
    process.exit(1);
  }

  try {
    await new GithubReleaser({
      owner: 'FocusriteGroup',
      repo: 'juce-end-to-end',
      tag: git.tag,
    }).release(paths.archive);
  } catch (error) {
    console.error(`Error releasing to GitHub: ${error}`);
    process.exit(1);
  }
};

release();
