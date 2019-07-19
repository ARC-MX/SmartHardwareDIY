// pages/connection/connection.js


function inArray(arr, key, val) {
  for (let i = 0; i < arr.length; i++) {
    if (arr[i][key] === val) {
      return i;
    }
  }
  return -1;
}

// ArrayBuffer转16进度字符串示例
function ab2hex(buffer) {
  var hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function (bit) {
      return ('00' + bit.toString(16)).slice(-2)
    }
  )
  return hexArr.join('');
}

Page({

  /**
   * 页面的初始数据
   */
  data: {
    currentDevice: [],
    connected: " ...",
    debugM: "页面跳转"
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function(options) {
    var bluetoothDevice = JSON.parse(options.currentTarget);
    this.setData({
      currentDevice: bluetoothDevice
    })
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function() {
    const deviceId= this.data.currentDevice.deviceId;
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        this.setData({
          connected: "已连接",
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        })
        //获取蓝牙设备服务
        this.getBLEDeviceServices(deviceId)
      },
      fail: function(res) {
        this.setData({
          connected: "连接失败",
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        })
      },
    })
  },


  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function() {

  },



  getBLEDeviceServices(deviceId) {
    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        console.log(deviceId)
        console.log("------获取蓝牙设备(" + deviceId + ")所有服务------\n", res)
        //遍历蓝牙服务列表
        for (let i = 0; i < res.services.length; i++) {
          //该服务是主服务
          if (res.services[i].isPrimary) {
            this.setData({
              debugM: "获取蓝牙设备" + deviceId + "主服务\n uuid:" + res.services[i].uuid,
            })
            //获取蓝牙设备某个服务中所有特征值
            this.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
            return
          }
        }
      }
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
        //遍历所有特征值
        for (let i = 0; i < res.characteristics.length; i++) {
          let item = res.characteristics[i]
          //该特征值支持的操作类型可读
          if (item.properties.read) {
            //读取低功耗蓝牙设备的特征值的二进制数据值
            wx.readBLECharacteristicValue({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
            })
          }
          //该特征值支持的操作类型可写
          if (item.properties.write) {
            this.setData({
              canWrite: true
            })
            this._deviceId = deviceId
            this._serviceId = serviceId
            this._characteristicId = item.uuid
            //写入操作
            this.writeBLECharacteristicValue()
          }
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
    })

    // 操作之前先监听，保证第一时间获取数据
    wx.onBLECharacteristicValueChange((characteristic) => {
      const idx = inArray(this.data.chs, 'uuid', characteristic.characteristicId)
      const data = {}
      if (idx === -1) {
        data[`chs[${this.data.chs.length}]`] = {
          uuid: characteristic.characteristicId,
          value: ab2hex(characteristic.value)
        }
      } else {
        data[`chs[${idx}]`] = {
          uuid: characteristic.characteristicId,
          value: ab2hex(characteristic.value)
        }
      }
      //data[`chs[${this.data.chs.length}]`] = {
      //   uuid: characteristic.characteristicId,
      //   value: "........"
      // }
      this.setData(data)
    })
  },
  
  stopBluetoothDevicesDiscovery: function (e) {
    wx.stopBluetoothDevicesDiscovery({
      success: (res) => {
        console.log("------停止搜寻附近的蓝牙外围设备------\n", res)
        this.setData({
          debugM: "停止搜寻附近的蓝牙外围设备\n" + res.errMsg
        });
      },
    })
  },

})