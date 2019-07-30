var utils = require('./utfEx.js');

const formatTime = date => {
  const year = date.getFullYear()
  const month = date.getMonth() + 1
  const day = date.getDate()
  const hour = date.getHours()
  const minute = date.getMinutes()
  const second = date.getSeconds()

  return [year, month, day].map(formatNumber).join('/') + ' ' + [hour, minute, second].map(formatNumber).join(':')
}

const formatNumber = n => {
  n = n.toString()
  return n[1] ? n : '0' + n
}


module.exports = {
  formatTime: formatTime
}

function inArray(arr, key, val) {
  for (let i = 0; i < arr.length; i++) {
    if (arr[i][key] === val) {
      return i;
    }
  }
  return -1;
}

// ArrayBuffer转16进制字符串示例
function u8Array2hex(buffer) {
  //创建一个新数组，使用 map 方法获取字符串中每个字符所对应的 ASCII 码组成的数组：
  var hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function(bit) {
      return ('00' + bit.toString(16)).slice(-2)
    }
  )
  return hexArr.join('');
}

function strToHexCharCode(str) {　　
  if (str === "")　　　　 return "";　　
  var hexCharCode = [];　　
  hexCharCode.push("0x");　　
  for (var i = 0; i < str.length; i++) {　　　　
    hexCharCode.push((str.charCodeAt(i)).toString(16));　　
  }　　
  return hexCharCode.join("");
}

//16进制转字符串

function hexCharCodeToStr(hexCharCodeStr) {　　
  console.log("hexCharCodeStr", hexCharCodeStr);
  var trimedStr = hexCharCodeStr.trim();　　//去除字符串开头结尾空格
  console.log("trimedStr", trimedStr);
  var rawStr = trimedStr.substr(0, 2).toLowerCase() === "0x"? trimedStr.substr(2):trimedStr;　　
  console.log("rawStr", rawStr, rawStr.length);
  var len = rawStr.length;　　
  if (len % 2 !== 0) {　　　　
    alert("Illegal Format ASCII Code!");　　　　
    return "";　　
  }　　
  var curCharCode;　　
  var resultStr = [];　　
  for (var i = 0; i < len; i = i + 2) {　　　　
    curCharCode = parseInt(rawStr.substr(i, 4), 16); // ASCII Code Value
    resultStr.push(String.fromCharCode(curCharCode));　　
  }　　
  console.log("resultStr", resultStr);
  return resultStr.join("");
}

module.exports = {
  inArray: inArray,
  u8Array2hex: u8Array2hex,
  strToHexCharCode: strToHexCharCode,
  hexCharCodeToStr: hexCharCodeToStr
}