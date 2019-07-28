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
    writeWords:0,
    writeSpeeds:0,
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
})