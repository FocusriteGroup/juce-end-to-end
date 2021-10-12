## Prerequisites

This project uses a variety of build tools. Most things can be accessed via an npm script
Before running anything you will need to do 

```
npm install
```

## Building on command line

```
npm run generate
npm run build
```

## Building a debug build on command line

```
npm run generate
npm run build-debug
```

## Making a project for _XCode_

```
npm run generate
```

## Opening in CLion

No need to generate. Just open the root of the project in CLion and it will find all the right CMake settings for you. See [here](./cmake.md) about auto formatting

## Opening in Visual Studio 2019

No need to generate. Just open the root of the project in VS2019 and it will find all the right CMake settings for you. You can, if required still generate the project using
```
npm run generate
```

