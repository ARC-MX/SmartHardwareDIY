// pages/characteristic/characteristic.js

var utils = require('../../utils/util.js');
var utfEx = require('../../utils/utfEx.js');
Page({

  /**
   * 页面的初始数据
   */
  data: {
    currentDevice: [],
    serviceID: "",
    characteristic: [],
    connectMessage: "已连接",
    connectFlag: "断开",
    addBrFlag: false,
    addSpaceFlag: true,
    readWords: 0,
    readSpeeds: 0,
    readValue: "",
    actionSheetHidden: false,
    readSpeedChecked: true,
    readHexChecked: false,
    writeWords: 0,
    writeSpeeds: 0,
    inputTime: 600,
    writeSpeedChecked: true,
    writeHexChecked: false,
    autoSendChecked: false,
    inputValue: "123",
    sendArrayBuffer: [],
    intervalID:"",
    icon: {
      normal: '../../utils/normal.png',
      active: '../../utils/active.png',
      disable: '../../utils/disable.png'
    }

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function(options) {
    var characteristic = JSON.parse(options.characteristic);
    var serviceID = options.serviceUUID;
    var device = JSON.parse(options.device);
    this.setData({
      currentDevice: device,
      serviceID: serviceID,
      characteristic: characteristic
    })

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function() {

  },
  //蓝牙连接控制
  changeConnectedState: function(e) {
    console.log("更改连接状态")
    if (this.data.connectMessage == "已连接") {
      wx.closeBLEConnection({
        deviceId: this.data.currentDevice.deviceId,
        success: res => {
          this.setData({
            connectMessage: "已断开",
            connectFlag: "连接"
          })
        },
      })

    } else {
      wx.createBLEConnection({
        deviceId: this.data.currentDevice.deviceId,
        success: res => {
          this.setData({
            connectMessage: "已连接",
            connectFlag: "断开"
          })
        },
      })
    }

  },
  //清空text内容
  textClean: function() {
    this.setData({
      readValue: ""
    })
  },
  //清空读取计数器
  readNumClean: function() {
    this.setData({
      readWords: 0
    })
  },

  //读取蓝牙回传信息
  readText: function() {
    wx.readBLECharacteristicValue({
      deviceId: this.data.currentDevice.deviceId,
      serviceId: this.data.serviceID,
      characteristicId: this.data.characteristic.uuid,
      success: res => {
        console.log("读值操作")
      },
    });

    wx.onBLECharacteristicValueChange((onNotityChangeRes) => {
      console.log('监听到特征值更新', onNotityChangeRes);
      this.readValueProcess(onNotityChangeRes.value);
    })
  },

  //设置显示速度使能
  readSpeedCheckChange: function(event) {
    this.setData({
      readSpeedChecked: event.detail
    });
  },

  //设置显示格式
  readHexCheckChange: function(event) {
    this.setData({
      readHexChecked: event.detail
    });
  },

  //设定长按监听事件--监听长按界面动作
  toggleActionSheet: function() {
    this.setData({
      actionSheetHidden: !this.data.actionSheetHidden
    });
  },

  writeSpeedCheckChange: function(event) {
    this.setData({
      writeSpeedChecked: event.detail
    });
  },

  //设定写入方式
  writeHexCheckChange: function(event) {
    var value = this.data.inputValue;
    //HEX显示
    if (event.detail) {
      value = utfEx.encode2utf8ArrayBuffer(value);
      value = utfEx.u8Array2string(value).toUpperCase();
    
    } else {
      value = utfEx.hexString2ArrayBuffer(value);
      value = utfEx.decode2utf8(value);
    }
    
    this.setData({
      writeHexChecked: event.detail,
      inputValue: value
    });
  },

  //设置自动写入
  writeAutoSendCheckChange: function(event) {
    this.setData({
      autoSendChecked: event.detail
    });
    var _this = this
    if (_this.data.autoSendChecked) {
      _this.data.intervalID = setInterval(_this.send, _this.data.inputTime);
      console.log("开启自动发送功能，定时" + _this.data.inputTime+" ms 发送一次")
    } else {
      clearInterval(_this.data.intervalID);
    }
  },

  //设定自动写入时间
  bindKeyInputTime: function(e) {
    this.setData({
      inputTime: e.detail.value,
    })
  },

  //设定写入数据
  bindKeyInputData: function(e) {
    this.setData({
      inputValue: e.detail.value
    })
  },

  //输入内容清空
  inputClean: function() {
    this.setData({
      inputValue: ""
    })
  },

  //写入字节数清零
  writeNumClean: function() {
    this.setData({
      writeWords: 0
    })
  },

  //向蓝牙发送字符串
  send: function() {
    this.writeValueProcess(this.data.inputValue);
    wx.writeBLECharacteristicValue({
      deviceId: this.data.currentDevice.deviceId,
      serviceId: this.data.serviceID,
      characteristicId: this.data.characteristic.uuid,
      value: this.data.sendArrayBuffer,
    })

  },

  //设置自动添加换行
  addBrEnable: function() {
    this.setData({
      addBrFlag: !this.data.addBrFlag
    })
  },

  //设置自动添加空格
  addSpaceEnable: function() {
    this.setData({
      addSpaceFlag: !this.data.addSpaceFlag
    })
  },

  //读取数据处理
  readValueProcess: function(characteristicValue) {
    var readValue = utfEx.u8Array2string(characteristicValue).toUpperCase();
    console.log("原始数据", readValue)
    if (!this.data.readHexChecked) {
      readValue = utfEx.decode2utf8(characteristicValue)
      console.log("UTF8解码", readValue)
    }
    readValue = this.data.addSpaceFlag ? readValue + "\t" : readValue;
    readValue = this.data.addBrFlag ? readValue + "\n" : readValue;
    this.setData({
      readValue: this.data.readValue + readValue,
      readWords: this.data.readWords + 1
    })
  },

  //写入数据处理
  writeValueProcess: function(value) {
    var sendArrayBuffer = [];
    if (!this.data.writeHexChecked) {
      sendArrayBuffer = utfEx.encode2utf8ArrayBuffer(value);
      console.log("当前以HEX形式发送：", sendArrayBuffer);
    } else {
      sendArrayBuffer = utfEx.hexString2ArrayBuffer(value);
      console.log("当前以字符串编码形式发送：", sendArrayBuffer);
    }
    this.setData({
      sendArrayBuffer: sendArrayBuffer,
    });
    console.log("HEX值等于：", value, "sendArrayBuffer值等于：", sendArrayBuffer)
  }
})