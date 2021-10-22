import fs from 'fs';

export const removeDirectory = (path) => {
  if (fs.existsSync(path)) {
    fs.rmdirSync(path, {recursive: true});
  }
};

export const createDirectory = (path) => {
  if (!fs.existsSync(path)) {
    fs.mkdirSync(path, {recursive: true});
  }
};
