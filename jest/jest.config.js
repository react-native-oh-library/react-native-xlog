/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
'use strict';

const path = require('path');

module.exports = {
  rootDir: path.resolve(__dirname, '..'),
  roots: [__dirname],
  testMatch: ['<rootDir>/jest/__tests__/*-test.js'],
  testEnvironment: 'node',
  testPathIgnorePatterns: ['/node_modules/', '/oh_modules/', '/harmony/'],
  globals: {__DEV__: true},
  setupFiles: ['<rootDir>/jest/jest.setup.js'],
  transformIgnorePatterns: ['node_modules/(?!react-native|@react-native|react)'],
  transform: {
    '^.+\\.js$': ['babel-jest', {
      configFile: false,
      babelrc: false,
      plugins: [['babel-plugin-syntax-hermes-parser', {parseLangTypes: 'flow'}], '@babel/plugin-transform-react-jsx'],
      presets: [
        ['@babel/preset-env', {targets: {node: 'current'}}],
        '@babel/preset-flow',
      ],
    }],
  },
  collectCoverageFrom: ['index.js'],
  coverageDirectory: '<rootDir>/jest/coverage',
  coverageThreshold: {global: {lines: 81, branches: 60}},
};


