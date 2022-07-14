let exp = {
  "transform": {
    ".(ts|tsx)": "ts-jest",
    "\\.wasm$": "jest-raw-loader",
  },
  "testRegex": "(/src/test/.*|\\.(test|spec))\\.(ts|tsx|js)$",
  "moduleFileExtensions": [
    "ts",
    "tsx",
    "js"
  ],
  "watchPlugins": [
      "jest-watch-typeahead/filename",
      "jest-watch-typeahead/testname"
    ],
};


module.exports = exp;