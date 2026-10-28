/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
'use strict';

const mockBridge = {
  open: jest.fn(),
  close: jest.fn(),
  log: jest.fn(),
  installUncaughtCrashHandler: jest.fn(),
  uninstallUncaughtCrashHandler: jest.fn(),
};

jest.mock('react-native', () => ({
  NativeModules: {XLogBridge: mockBridge},
  NativeAppEventEmitter: {},
  DeviceEventEmitter: {},
  Platform: {OS: 'harmony'},
}), {virtual: true});

const XLog = require('../../index');

beforeEach(() => {
  jest.clearAllMocks();
  for (const method of Object.values(mockBridge)) {
    method.mockReset().mockResolvedValue(undefined);
  }
});

describe('XLog lifecycle', () => {
  test.each(['open', 'close'])('%s calls the native method and forwards its result', async method => {
    mockBridge[method].mockResolvedValue('result');
    await expect(XLog[method]()).resolves.toBe('result');
    expect(mockBridge[method]).toHaveBeenCalledTimes(1);
    expect(mockBridge[method]).toHaveBeenCalledWith();
  });

  test.each(['open', 'close'])('%s propagates native errors', async method => {
    const error = new Error('native failure');
    mockBridge[method].mockRejectedValue(error);
    await expect(XLog[method]()).rejects.toBe(error);
  });
});

describe('XLog log levels', () => {
  test.each([
    ['verbose', 0], ['debug', 1], ['info', 2],
    ['warn', 3], ['error', 4], ['fatal', 5],
  ])('%s forwards its level, tag and message', async (method, level) => {
    mockBridge.log.mockResolvedValue('written');
    await expect(XLog[method]('test tag', 'test message')).resolves.toBe('written');
    expect(mockBridge.log).toHaveBeenCalledTimes(1);
    expect(mockBridge.log).toHaveBeenCalledWith(level, 'test tag', 'test message');
  });

  test.each(['verbose', 'debug', 'info', 'warn', 'error', 'fatal'])(
    '%s propagates native errors', async method => {
      const error = new Error('write failed');
      mockBridge.log.mockRejectedValue(error);
      await expect(XLog[method]('tag', 'message')).rejects.toBe(error);
    },
  );
});

describe('XLog crash handler', () => {
  test.each(['installUncaughtCrashHandler', 'uninstallUncaughtCrashHandler'])(
    '%s calls the native method', async method => {
      await expect(XLog[method]()).resolves.toBeUndefined();
      expect(mockBridge[method]).toHaveBeenCalledTimes(1);
      expect(mockBridge[method]).toHaveBeenCalledWith();
    },
  );

  test.each(['installUncaughtCrashHandler', 'uninstallUncaughtCrashHandler'])(
    '%s is a no-op when the native method is unavailable', async method => {
      const original = mockBridge[method];
      try {
        mockBridge[method] = undefined;
        await expect(XLog[method]()).resolves.toBeUndefined();
        expect(original).not.toHaveBeenCalled();
      } finally {
        mockBridge[method] = original;
      }
    },
  );

  test.each(['installUncaughtCrashHandler', 'uninstallUncaughtCrashHandler'])(
    '%s propagates native errors', async method => {
      const error = new Error('handler failure');
      mockBridge[method].mockRejectedValue(error);
      await expect(XLog[method]()).rejects.toBe(error);
    },
  );
});

