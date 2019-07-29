// pages/characteristic/characteristic.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    currentDevice: [],
    buletoothName:"",
    connectMessage:"已连接",
    readWords:0,
    readSpeeds:0,
    readValue:'',
    readSpeedChecked:true,
    readHexChecked:true,
    writeWords:0,
    writeSpeeds:0,
    inputTime:60,
    writespeedChecked: true,
    writeHexChecked: true,
    autoSendChecked:false,
    inputValue:"",
    connectFlag:"断开",
    characteristic: []
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var characteristic = JSON.parse(options.characteristic);
    var device = JSON.parse(options.device);
    this.setData({
      currentDevice: device,
      characteristic: characteristic
    })

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },

  changeConnectedState: function () {

  },
  bindKeyInput: function () {

  },
  readSpeedCheckChange: function (event) {
    this.setData({
      readSpeedChecked: event.detail
    });
  },
  readHexCheckChange: function (event) {
    this.setData({
      readHexChecked: event.detail
    });
  },
  writeSpeedCheckChange: function (event) {
    this.setData({
      writeSpeedChecked: event.detail
    });
  },
  writeHexCheckChange: function (event) {
    this.setData({
      writeHexChecked: event.detail
    });
  },
  writeAutoSendCheckChange: function (event) {
    this.setData({
      autoSendChecked: event.detail
    });
    if(autoSendChecked){
      var intervalID = setInterval(function () {
        that.send()
      }, inputTime);
    }else{
      clearInterval(intervalID);
    }

  },
  textClean: function () {
    this.setData({
      inputValue: ""
    })
  }, 
  readNumClean: function () {
    this.setData({
      readWords: 0
    })
  },

  readText: function () {

    //读取蓝牙回传信息
  },   

  bindKeyInputTime: function (e) {
    this.setData({
      inputTime: e.detail.value
    })
  },

  bindKeyInputData: function (e) {
    this.setData({
      inputValue: e.detail.value
    })
  },

  //输入内容清空
 inputClean: function () {
   this.setData({
     inputValue: ""
   })
  },

  //写入字节数清零
  writeNumClean: function () {
    this.setData({
      writeWords:0
    })
  },

  //向蓝牙发送字符串
  send: function () {

  },   

})