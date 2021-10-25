import fs from 'fs';

export const removeDirectory = (path: string) => {
  if (fs.existsSync(path)) {
    fs.rmdirSync(path, {recursive: true});
  }
};

export const createDirectory = (path: string) => {
  if (!fs.existsSync(path)) {
    fs.mkdirSync(path, {recursive: true});
  }
};
