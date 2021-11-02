import git from 'git-rev-sync';

export default {
  hash: {
    short: git.short(),
    long: git.long(),
  },
  branch: git.branch(),
  tag: git.tag(),
  isOnTag: !git.isTagDirty(),
};
