const chalk = require("chalk");

const commands = [
  {
    command: "help",
    arguments: null,
    description: "Show this help documentation",
    generalUsage: true,
    dependencies: null,
    equivalent: null,
  },
  {
    command: "clean",
    arguments: null,
    description: "Remove all build directories and start from scratch",
    generalUsage: true,
    dependencies: null,
    equivalent: null,
  },
  {
    command: "generate",
    arguments: null,
    description: "Generate the projects as required for the platform IDE",
    generalUsage: true,
    dependencies: null,
    equivalent: null,
  },
  {
    command: "build",
    arguments: null,
    description: "Build a release version",
    generalUsage: true,
    dependencies: ["npm run generate"],
    equivalent: null,
  },
  {
    command: "build-debug",
    arguments: null,
    description: "Build a debug version",
    generalUsage: true,
    dependencies: ["npm run generate"],
    equivalent: null,
  },
  {
    command: "test",
    arguments: null,
    description: "Run all the unit tests in release",
    generalUsage: true,
    dependencies: ["npm run generate", "npm run build"],
    equivalent: null,
  },
  {
    command: "test-debug",
    arguments: null,
    description: "Run all the unit tests in debug",
    generalUsage: true,
    dependencies: ["npm run generate", "npm run build-debug"],
    equivalent: null,
  },
  {
    command: "all",
    arguments: null,
    description: "Run all in release",
    generalUsage: true,
    dependencies: null,
    equivalent: ["npm run clean", "npm run generate", "npm run build", "npm run test"],
  },
  {
    command: "all-debug",
    arguments: null,
    description: "Run all in debug",
    generalUsage: true,
    dependencies: null,
    equivalent: ["npm run clean", "npm run generate", "npm run build-debug", "npm run test-debug"],
  },
  {
    command: "validate-circle",
    arguments: null,
    description: "Validate the circle config.yml",
    generalUsage: true,
    dependencies: null,
    equivalent: null,
  },
  {
    command: "lint",
    arguments: null,
    description: "Lints the e2e tests",
    generalUsage: false,
    dependencies: null,
    equivalent: null,
  },
  {
    command: "format",
    arguments: null,
    description: "Run clang-format over the whole C++ codebase",
    generalUsage: false,
    dependencies: null,
    equivalent: null,
  },
];

const printCommand = (scriptCommand) => {
  console.log(
    chalk.green.bold(scriptCommand.command) +
      " " +
      scriptCommand.description +
      (scriptCommand.generalUsage ? "" : " " + chalk.red.underline("[Not for general use]"))
  );
};

const printArguments = (scriptCommand) => {
  if (scriptCommand.arguments === null) {
    return;
  }
  console.log(chalk.blue("Arguments:"));
  scriptCommand.arguments.forEach((argument) => {
    console.log("  [" + argument.argument + "] " + argument.description);
  });
};

const printEquivalents = (scriptCommand) => {
  if (scriptCommand.equivalent === null) {
    return;
  }
  console.log(chalk.blue("Equivalents:"));
  scriptCommand.equivalent.forEach((equiv) => console.log("  " + equiv));
};

const printDependencies = (scriptCommand) => {
  if (scriptCommand.dependencies === null) {
    return;
  }
  console.log(chalk.blue("Dependencies:"));
  scriptCommand.dependencies.forEach((depends) => console.log("  " + depends));
};

console.clear();
commands.forEach((scriptCommand) => {
  printCommand(scriptCommand);
  printArguments(scriptCommand);
  printDependencies(scriptCommand);
  printEquivalents(scriptCommand);
  console.log("");
});
