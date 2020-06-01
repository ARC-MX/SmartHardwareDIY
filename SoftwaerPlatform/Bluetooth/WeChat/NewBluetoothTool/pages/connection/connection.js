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
    currentDeviceServices: [],
    activeService: "",
    serviceType: "自定义",
    currentCharacteristics: [],
    activeName: "",
    connected: " ..."
  },

  _currentOperation: "",

  //监听蓝牙连接状态
  onBLEConnectionStateChange: function() {
    wx.onBLEConnectionStateChange(res => {
      this.setData({
        connected: res.connected ? "已连接" : "已断开"
      })
      console.log("蓝牙连接状态为：", res.connected)
      if (!res.connected) {
        console.log("页面跳转", res.connected)
        wx.navigateTo({
          url: '../index/index',
        })
      }

    })
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
        })
        //获取蓝牙设备服务
        this.getBLEDeviceServices(deviceId)
      },
      fail: res => {
        this.setData({
          connected: "连接失败"
        })
      },
    })
    this.onBLEConnectionStateChange();
  },

  /**
   * //获取蓝牙服务
   */
  getBLEDeviceServices(deviceId) {
    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        console.log("------获取蓝牙设备(" + deviceId + ")所有服务------\n", res);
        var services = res.services;
        if (services.length > 0) {
          this.onBLECharacteristicValueChange();
        }
        for (let i = 0; i < services.length; i++) {
          switch (services[i].uuid.substring(4, 8)) {
            case "1800":
              services[i]["serviceType"] = "通用访问";
              break;
            case "1801":
              services[i]["serviceType"] = "通用属性";
              break;
            case "180A":
              services[i]["serviceType"] = "设备信息";
              break;
            case "181C":
              services[i]["serviceType"] = "用户数据";
              break;
            case "1805":
              services[i]["serviceType"] = "当前时间";
              break;
            default:
              services[i]["serviceType"] = "自定义";
          }
        }
        console.log("services 为：", services);
        console.log("res.services 为：", res.services);
        this.setData({
          currentDeviceServices: services
        });
        console.log("currentDeviceServices 为：", this.data.currentDeviceServices);
      }
    })
  },

  //监听低功耗蓝牙设备的特征值变化事件
  onBLECharacteristicValueChange: function() {
    // 操作之前先监听，保证第一时间获取数据
    wx.onBLECharacteristicValueChange((characteristic) => {
      const index = utfEx.inArray(this.data.currentCharacteristics, 'uuid', characteristic.characteristicId)
      if (index === -1) {
        var currentCharacteristics = this.data.currentCharacteristics[this.data.currentCharacteristics.length];

        if (this._currentOpreation == "notify") {
          console.log("currentCharacteristics[notifyValue] 为：", characteristic.value);
          //currentCharacteristics["notifyValue"] = characteristic.value;
          currentCharacteristics["notifyValue"] = utfEx.ab2hex(characteristic.value);
          this._currentOpreation = "";
        } else if (this._currentOpreation == "read") {
          //currentCharacteristics["readValue"] = characteristic.value;
          currentCharacteristics["readValue"] = utfEx.ab2hex(characteristic.value);
          this._currentOpreation = "";
        }

        this.data.currentCharacteristics[this.data.currentCharacteristics.length] = currentCharacteristics;
      } else {
        var currentCharacteristics = this.data.currentCharacteristics[index];

        if (this._currentOpreation == "notify") {
          currentCharacteristics["notifyValue"] = utfEx.ab2hex(characteristic.value);
          //currentCharacteristics["notifyValue"] = characteristic.value;
          this._currentOpreation = "";
        } else if (this._currentOpreation == "read") {
          //currentCharacteristics["readValue"] = characteristic.value;
          currentCharacteristics["readValue"] = utfEx.ab2hex(characteristic.value);
          this._currentOpreation = "";
        }

        this.data.currentCharacteristics[index] = currentCharacteristics;
      }
      this.setData({
        currentCharacteristics: this.data.currentCharacteristics
      })
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
          let characteristic = res.characteristics[i];
          this.getCharacteristicNotify_IndicateValue(deviceId, serviceId, characteristic);
        }
      },
      fail(res) {
        console.error("------获取蓝牙设备(" + deviceId + ")主服务(" + serviceId + ")特征值失败------", res)
      }
    })
  },

  stopBluetoothDevicesDiscovery: function(e) {
    wx.stopBluetoothDevicesDiscovery({
      success: (res) => {
        console.log("------停止搜寻附近的蓝牙外围设备------\n", res)
      },
    })
  },

  onChange(event) {
    if (event.detail === "") {
      this.setData({
        activeName: ""
      });
      return;
    }
    this.getBLEDeviceCharacteristics(this.data.deviceId, this.data.currentDeviceServices[event.detail].uuid);
    this.setData({
      activeName: event.detail,
      activeService: this.data.currentDeviceServices[event.detail]
    });
  },

  getNotifyValue: function(e) {
    var characteristic = e.currentTarget.dataset.characteristic
    const deviceId = this.data.currentDevice.deviceId;
    const serviceId = this.data.activeService.uuid;
    if (characteristic.properties.read) {
      wx.readBLECharacteristicValue({
        deviceId,
        serviceId,
        characteristicId: characteristic.uuid,
      })
      this._currentOpreation = "read";
      console.log("this._currentOpreation = ", this._currentOpreation)
    }
  },

  writeValue: function(e) {
    var characteristic = e.currentTarget.dataset.characteristic
    const deviceId = this.data.currentDevice.deviceId;
    const serviceId = this.data.activeService.uuid;
    wx.navigateTo({
      url: '../characteristic/characteristic?characteristic=' + JSON.stringify(characteristic) + '&serviceUUID=' + serviceId + '&device=' + JSON.stringify(this.data.currentDevice)
    })

  },

  getCharacteristicNotify_IndicateValue(deviceId, serviceId, characteristic) {
    //该特征值是否支持 notify（通知） 或 indicate（指示） 操作
    if (characteristic.properties.notify || characteristic.properties.indicate) {
      wx.notifyBLECharacteristicValueChange({
        deviceId,
        serviceId,
        characteristicId: characteristic.uuid,
        state: true,
      })
      this._currentOpreation = "notify";
    }

  }


})