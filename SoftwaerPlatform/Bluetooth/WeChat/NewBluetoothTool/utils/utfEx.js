/* utf.js - UTF-8 <=> UTF-16 convertion
 *
 * Copyright (C) 1999 Masanao Izumo <iz@onicos.co.jp>
 * Version: 1.0
 * LastModified: Dec 25 1999
 * This library is free.  You can redistribute it and/or modify it.
 */

/*
 * Interfaces:
 * utf8 = utf16to8(utf16);
 * utf16 = utf16to8(utf8);
 */
// ArrayBuffer转16进制字符串示例

function inArray(arr, key, val) {
  for (let i = 0; i < arr.length; i++) {
    if (arr[i][key] === val) {
      return i;
    }
  }
  return -1;
}

function u8Array2string(buffer) {
  //创建一个新数组，使用 map 方法获取字符串中每个字符所对应的 ASCII 码组成的数组：
  var hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function(bit) {
      return ('00' + bit.toString(16)).slice(-2)
    }
  )
  return hexArr.join('');
}

function hexString2ArrayBuffer(hexString) {
  var pos = 0;
  var len = hexString.length;
  if (len % 2 != 0) {
    return null;
  }
  len /= 2;
  var hexA = new Array();
  for (var i = 0; i < len; i++) {
    var s = hexString.substr(pos, 2);
    var v = parseInt(s, 16);
    hexA.push(v);
    pos += 2;
  }
  return new Uint8Array(hexA).buffer;
}

function decode2utf8(arr) {
  if (typeof arr === 'string') {
    return arr;
  }
  var unicodeString = '';
  var _arr = new Uint8Array(arr);
  for (var i = 0; i < _arr.length; i++) {
    var one = _arr[i].toString(2);
    var v = one.match(/^1+?(?=0)/);

    if (v && one.length === 8) {
      var bytesLength = v[0].length;
      var store = _arr[i].toString(2).slice(7 - bytesLength);
      for (var st = 1; st < bytesLength; st++) {
        store += _arr[st + i].toString(2).slice(2)
      }
      unicodeString += String.fromCharCode(parseInt(store, 2));
      i += bytesLength - 1;
    } else {
      unicodeString += String.fromCharCode(_arr[i]);
    }
  }
  return unicodeString
}

function encode2utf8ArrayBuffer(str) {
  var result = new Array();
  var k = 0;
  for (var i = 0; i < str.length; i++) {
    var j = encodeURI(str[i]);
    if (j.length == 1) {
      // 未转换的字符
      result[k++] = j.charCodeAt(0);
    } else {
      // 转换成%XX形式的字符
      var bytes = j.split("%");
      for (var l = 1; l < bytes.length; l++) {
        result[k++] = parseInt("0x" + bytes[l]);
      }
    }
  }
  return new Uint8Array(result).buffer;
}

module.exports = {
  inArray: inArray,
  u8Array2string: u8Array2string,
  hexString2ArrayBuffer: hexString2ArrayBuffer,
  decode2utf8: decode2utf8,
  encode2utf8ArrayBuffer: encode2utf8ArrayBuffer
}