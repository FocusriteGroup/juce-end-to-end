const git = require('git-rev-sync');

module.exports = {
  hash: {
    short: git.short(),
    long: git.long(),
  },
  branch: git.branch(),
  tag: git.tag(),
  isOnTag: !git.isTagDirty(),
};
