const path = require('path');
const fs = require('fs');
const process = require('process');
const {Octokit} = require('octokit');

const validateTag = (tagName) => {
  if (!/^v\d*\.\d*\.\d*$/i.test(tagName)) {
    throw new Error(`Invalid tag : ${tagName}`);
  }
};

class GithubReleaser {
  constructor(releaseInfo) {
    validateTag(releaseInfo.tag);
    this.tag = releaseInfo.tag;
    this.owner = releaseInfo.owner;
    this.repo = releaseInfo.repo;
    this.api = new Octokit({auth: process.env.GIT_ACCESS_TOKEN}).rest.repos;
  }

  releaseAll = async (artefacts) => {
    for await (const artefact of artefacts) {
      await this.release(artefact);
    }
  };

  release = async (artefact) => {
    const releaseId = await this.#findOrCreateRelease();
    await this.#deleteOldArtefact(releaseId, artefact);
    await this.#uploadAsset(artefact, releaseId);
  };

  #log = (str) => console.log(str);

  #findOrCreateRelease = async () => {
    const releases = await this.api.listReleases({
      owner: this.owner,
      repo: this.repo,
    });

    if (!releases.data.length) {
      return await this.#createRelease();
    }

    const release = releases.data.find(
      (release) => release.tag_name === this.tag
    );

    if (!release) {
      return await this.#createRelease();
    }

    this.#log('Found release');
    return release.id;
  };

  #createRelease = async () => {
    try {
      const release = await this.api.createRelease({
        owner: this.owner,
        repo: this.repo,
        tag_name: this.tag,
      });

      this.#log('Created release');
      return release.data.id;
    } catch (err) {
      this.#log(`Failed to create release`);
    }
  };

  #deleteAsset = async (artefact, asset) => {
    try {
      await this.api.deleteReleaseAsset({
        owner: this.owner,
        repo: this.repo,
        asset_id: asset.id,
      });
      this.#log(`Removed old artefact ${artefact}`);
    } catch (err) {
      this.#log(`Failed to remove ${artefact} : ${err}`);
    }
  };

  #deleteOldArtefact = async (releaseId, artefact) => {
    try {
      const duplicateAssets = (
        await this.api.listReleaseAssets({
          owner: this.owner,
          repo: this.repo,
          release_id: releaseId,
        })
      ).data.filter((asset) => asset.name === path.basename(artefact));

      for await (const asset of duplicateAssets) {
        await this.#deleteAsset(artefact, asset);
      }
    } catch (err) {
      this.#log(`Failed to list releases : ${err}`);
    }
  };

  #uploadAsset = async (artefact, releaseId) => {
    try {
      await this.api.uploadReleaseAsset({
        owner: this.owner,
        repo: this.repo,
        release_id: releaseId,
        name: path.basename(artefact),
        data: fs.readFileSync(artefact),
      });
      this.#log(`Uploaded artefact ${artefact}`);
    } catch (err) {
      this.#log(`Failed to upload ${artefact}`);
    }
  };
}

module.exports = {GithubReleaser};
