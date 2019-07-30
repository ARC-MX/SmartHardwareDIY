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
    addSpaceFlag:true,
    readWords: 0,
    readSpeeds: 0,
    readValue: '',
    actionSheetHidden: false,
    readSpeedChecked: true,
    readHexChecked: false,
    readUtfChecked:true,
    readUtf: "UTF16",
    writeWords: 0,
    writeSpeeds: 0,
    inputTime: 60,
    writeSpeedChecked: true,
    writeHexChecked: true,
    writeUtfChecked: true,
    writeUtf:"UTF16",
    autoSendChecked: false,
    inputValue: ""

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
    if(this.data.readHexChecked){
      var readValue = utils.strToHexCharCode(this.data.readValue)
    }else{
      var readValue = utils.hexCharCodeToStr(this.data.readValue)
    }
    this.setData({
      readValue: readValue,
    })
    return readValue
  },

  //设置UTF格式
  readUtfChange: function (event) {
    this.setData({
      readUtfChecked: event.detail
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
  writeHexCheckChange: function(event) {
    this.setData({
      writeHexChecked: event.detail
    });
  },
  //设置UTF格式
  writeUtfChange: function (event) {
    this.setData({
      writeUtfChecked: event.detail
    });
  },
  writeAutoSendCheckChange: function(event) {
    this.setData({
      autoSendChecked: event.detail
    });
    if (autoSendChecked) {
      var intervalID = setInterval(function() {
        that.send()
      }, inputTime);
    } else {
      clearInterval(intervalID);
    }

  },

  bindKeyInputTime: function(e) {
    this.setData({
      inputTime: e.detail.value
    })
  },

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

  readValueProcess: function (characteristicValue){
    var readValue = utils.u8Array2hex(characteristicValue).toUpperCase();
    if(this.data.readHexChecked){
      readValue = utils.hexCharCodeToStr(readValue)
    }
    readValue = this.data.addSpaceFlag ? readValue + "\t" : readValue;
    readValue = this.data.addBrFlag ? readValue + "\n" : readValue;
    this.setData({
      readValue: this.data.readValue + readValue,
      readWords: this.data.readWords + 1
    })
  }
})