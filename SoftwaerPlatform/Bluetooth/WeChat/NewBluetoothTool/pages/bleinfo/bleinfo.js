// pages/bleinfo/bleinfo.js

var utfEx = require('../../utils/utfEx.js');

Page({
  /**
   * 页面的初始数据
   */
  data: {
    actionSheetHidden: false,
    filterEnableChecked: false,
    discoveryFlag: true,
    hasBluetoothDivices: false,
    connected: false,
    chs: [],
    bluetoothDevices: []
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function(options) {
    //搜索蓝牙设备   包含初始化蓝牙
    wx.openBluetoothAdapter({ //初始化蓝牙模块
      //初始化成功，开始搜寻附近的蓝牙外围设备
      success: res => {
        console.log("------初始化蓝牙模块成功------\n", res)
        this.startBluetoothDevicesDiscovery()
        this.setData({
          discoveryFlag: false
        });
      },
      //初始化失败，如果失败原因是 “当前蓝牙适配器不可用”那么监听蓝牙状态是否改变
      //当蓝牙适配器是可用时再次开始搜寻附近的蓝牙外围设备
      fail: res => {
        console.log("------初始化蓝牙模块故障，尝试解决------\n", res)
        if (res.errCode === 10001) {
          wx.onBluetoothAdapterStateChange(res => {
            console.log("------监听蓝牙适配器状态变化事件------\n", res)
            if (res.available) {
              this.startBluetoothDevicesDiscovery()
            }
          })
        } else {
          console.log("------初始化蓝牙模块故障，解决失败------\n", res)
          this.setData({
            hasBluetoothDivices: true
          });
        }
      },
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
 * 页面相关事件处理函数--监听用户下拉动作
 */
  onPullDownRefresh: function () {

  },

  /**
   * 设定长按监听事件--监听长按界面动作
   */
  toggleActionSheet: function() {
    console.info("长按触发");
    this.setData({
      actionSheetHidden: !this.data.actionSheetHidden
    });
  },
  /**
   * 设定主服务过滤，只显示含有主服务的BLE设备
   */
  filterEnable: function() {
    this.setData({
      filterEnableChecked: !this.data.filterEnableChecked
    })
    console.log(this.data.filterEnableChecked ? "设定蓝牙主服务过滤使能" : "设定蓝牙主服务过滤关闭")

    var devices_r = [];
    var devices = this.data.bluetoothDevices;

    for (let index in devices) {
      if (this.data.filterEnableChecked) {
      if (this.filterEachDevice(devices[index]))
        devices_r.push(devices[index]);
    }    else{
      devices_r.push(devices[index]);
    }
  }
    this.setData({
      bluetoothDevices: devices_r
    })


  },

  //搜寻附近的蓝牙外围设备
  startBluetoothDevicesDiscovery: function() {
    if (this._discoveryStarted) { //设置_搜索标志，如果正在搜索则跳过搜索过程
      return
    }
    this._discoveryStarted = true
    wx.startBluetoothDevicesDiscovery({
      services: [], //搜索特定设备
      allowDuplicatesKey: true,
      interval: 0, //上报设备间隔
      success: res => {
        console.log("------开始搜寻附近的蓝牙外围设备..------\n", res);
        this.onBluetoothDeviceFound(); //监听寻找到新设备的事件
      }
    })
  },
  //停止搜寻附近的蓝牙外围设备
  stopBluetoothDevicesDiscovery() {
    wx.stopBluetoothDevicesDiscovery()
  },

  //监听寻找到新设备的事件
  onBluetoothDeviceFound: function() {
    wx.onBluetoothDeviceFound(res => {
      res.devices.forEach(device => {
        if (this.filterEachDevice(device)) {
          console.log("------监听寻找到新设备的事件------\n",device.advertisServiceUUIDs.length)
          const foundDevices = this.data.bluetoothDevices
          const idx = utfEx.inArray(foundDevices, 'deviceId', device.deviceId) //判断当前设备是否列表中
          if (idx === -1) { //当前设备不在已有列表中，向列表中添加当前设备
            this.data.bluetoothDevices[foundDevices.length] = device

          } else { //当前设备在已有列表中，向列表中更新当前设备

            this.data.bluetoothDevices[idx] = device
          }
          this.setData({
            bluetoothDevices: this.data.bluetoothDevices
          })
        }
      })
    })
  },

  //查看设备是否包含服务
  filterEachDevice: function(device) {
    //排除设备名称不一致的情况
    if (this.data.filterEnableChecked) {
      console.log("------过滤无服务设备------\n", device)
      if (device.advertisServiceUUIDs.length == 0) {
        console.log("------无服务设备已过滤------\n", device)
        return false;
      }
    }
    return true;
  },

  //创建蓝牙连接页面
  createBLEConnectionPage: function(e) {
    var index = e.currentTarget.dataset.index
    //将对象转为string
    var bluetoothDevice = JSON.stringify(this.data.bluetoothDevices[index])
    this.stopBluetoothDevicesDiscovery();
    wx.navigateTo({
      url: '../connection/connection?currentTarget=' + bluetoothDevice
    })
  }


})