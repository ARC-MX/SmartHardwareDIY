// pages/connection/connection.js
var utfEx = require('../../utils/utfEx.js');
Page({

  /**
   * 页面的初始数据
   */
  data: {
    deviceId: "",
    currentDevice: [],
    deviceAdvertisData: "",
    currentDeviceService: [],
    activeServiceId: "",
    servicesType: "自定义",
    currentCharacteristics: [],
    activeName: "",
    connected: " ...",
    debugM: "页面跳转"
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function(options) {
    var bluetoothDevice = JSON.parse(options.currentTarget);
    var deviceAdvertisData = utfEx.decode2utf8(bluetoothDevice.advertisData) //当前蓝牙设备的广播数据段中的 ManufacturerData 数据段
    this.setData({
      currentDevice: bluetoothDevice,
      deviceAdvertisData: deviceAdvertisData
    })
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function() {
    const deviceId = this.data.currentDevice.deviceId;
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        this.setData({
          deviceId: deviceId,
          connected: "已连接",
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        })
        //获取蓝牙设备服务
        this.getBLEDeviceServices(deviceId)
      },
      fail: res => {
        this.setData({
          connected: "连接失败",
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        })
      },
    })
  },

  /**
   * //获取蓝牙服务
   */
  getBLEDeviceServices(deviceId) {
    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        console.log("------获取蓝牙设备(" + deviceId + ")所有服务------\n", res)
        this.setData({
          currentDeviceService: res.services
        })
      }
    })
  },

  //监听低功耗蓝牙设备的特征值变化事件
  onBLECharacteristicValueChange:function(){
    // 操作之前先监听，保证第一时间获取数据
    wx.onBLECharacteristicValueChange((characteristic) => {
      const index = utfEx.inArray(this.data.currentCharacteristics, 'uuid', characteristic.characteristicId)
      if (index === -1) {

        var currentCharacteristics = this.data.currentCharacteristics[this.data.currentCharacteristics.length];
        currentCharacteristics["value"] = utfEx.decode2utf8(characteristic.value);
        this.data.currentCharacteristics[this.data.currentCharacteristics.length] = currentCharacteristics;
        console.log("currentCharacteristics :", currentCharacteristics)
      } else {
        var currentCharacteristics = this.data.currentCharacteristics[index];
        currentCharacteristics["value"] = utfEx.decode2utf8(characteristic.value);
        this.data.currentCharacteristics[index] = currentCharacteristics;
        console.log("currentCharacteristics :", currentCharacteristics)
      }
      this.setData({
        currentCharacteristics: this.data.currentCharacteristics
      }
      )
    })
  },

  //获取蓝牙特征值对应服务
  getBLEDeviceCharacteristics(deviceId, serviceId) {
    //获取蓝牙设备某个服务中所有特征值
    wx.getBLEDeviceCharacteristics({
      deviceId,
      serviceId,
      success: (res) => {
        console.log("------获取蓝牙设备(" + deviceId + ")主服务(" + serviceId + ")特征值------\n", res, res.characteristics)
        this.setData({ //记录当前服务下的所有特征值
          currentCharacteristics: res.characteristics
        })

        //遍历所有特征值
        for (let i = 0; i < res.characteristics.length; i++) {
          let item = res.characteristics[i]
          //该特征值是否支持 notify（通知） 或 indicate（指示） 操作
          if (item.properties.notify || item.properties.indicate) {
            wx.notifyBLECharacteristicValueChange({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
              state: true,
            })
          }
        }
      },
      fail(res) {
        console.error("------获取蓝牙设备(" + deviceId + ")主服务(" + serviceId + ")特征值失败------", res)
      }
    }),
      this.onBLECharacteristicValueChange()
  },

  stopBluetoothDevicesDiscovery: function(e) {
    wx.stopBluetoothDevicesDiscovery({
      success: (res) => {
        console.log("------停止搜寻附近的蓝牙外围设备------\n", res)
        this.setData({
          debugM: "停止搜寻附近的蓝牙外围设备\n" + res.errMsg
        });
      },
    })
  },

  onChange(event) {
    this.getBLEDeviceCharacteristics(this.data.deviceId, this.data.currentDeviceService[event.detail].uuid)
    this.setData({
      activeName: event.detail,
      activeServiceId: this.data.currentDeviceService[event.detail]
    });
  },

  selectCharacteristic: function(e) {
    var characteristic = e.currentTarget.dataset.characteristic
    //将对象转为string
    wx.navigateTo({
      url: '../characteristic/characteristic?characteristic=' + JSON.stringify(characteristic) + '&serviceUUID=' + this.data.activeServiceId.uuid + '&device=' + JSON.stringify(this.data.currentDevice)
    })
  }


})