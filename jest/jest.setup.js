/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
'use strict';

global.nativeModuleProxy = global.nativeModuleProxy || {};
if (typeof global.queueMicrotask === 'undefined') {
  global.queueMicrotask = callback => Promise.resolve().then(callback);
}
