//index.js
//获取应用实例
const app = getApp()


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
  data: {
    debugM: "请获取蓝牙设备",
    discoveryFlag: true,
    hasBluetoothDivices: false,
    connected: false,
    chs: [],
    bluetoothDevices: []
    
  },



  //事件处理函数
  bindViewTap: function() {
    wx.navigateTo({
      url: '../logs/logs'
    })
  },



  openBluetoothAdapter: function(e) {
    var self = this; //转存this对象
    //搜索蓝牙设备   包含初始化蓝牙
    wx.openBluetoothAdapter({ //初始化蓝牙模块
      //初始化成功，开始搜寻附近的蓝牙外围设备
      success: function(res) {
        console.log("------初始化蓝牙模块成功------\n", res)
        self.startBluetoothDevicesDiscovery()
        self.setData({
          debugM: "初始化蓝牙模块成功\n" + res.errMsg,
          discoveryFlag: false
        });
      },
      //初始化失败，如果失败原因是 “当前蓝牙适配器不可用”那么监听蓝牙状态是否改变
      //当蓝牙适配器是可用时再次开始搜寻附近的蓝牙外围设备
      fail: function(res) {
        console.log("------初始化蓝牙模块故障，尝试解决------\n", res)
        self.setData({
          debugM: "初始化蓝牙模块故障，尝试解决\n" + res.errMsg
        });
        if (res.errCode === 10001) {
          wx.onBluetoothAdapterStateChange(function(res) {
            console.log("------监听蓝牙适配器状态变化事件------\n", res)
            self.setData({
              debugM: "监听蓝牙适配器状态变化事件"
            });
            if (res.available) {
              self.setData({
                debugM: "初始化蓝牙模块故障以解决\n" + res.errMsg
              });
              self.startBluetoothDevicesDiscovery()
            }
          })
        } else {

          console.log("------初始化蓝牙模块故障，解决失败------\n", res)
          self.setData({
            debugM: "初始化蓝牙模块失败\n    请打开蓝牙\n" + res.errMsg,
            hasBluetoothDivices: true
          });
        }
      },
      complete: function(res) {
        console.log("------初始化蓝牙模块结束------\n",res)
      },
    })
  },

  startBluetoothDevicesDiscovery: function() {
    //设置_搜索标志，如果正在搜索则跳过搜索过程
    if (this._discoveryStarted) {
      return
    }
    this._discoveryStarted = true
    var self = this
    //搜寻附近的蓝牙外围设备
    wx.startBluetoothDevicesDiscovery({
      services: [],  //搜索特定设备
      allowDuplicatesKey: true,
      interval: 0,  //上报设备间隔
      success: function(res) {
        console.log("------开始搜寻附近的蓝牙外围设备..------\n",res)
        self.setData({
          debugM: "开始搜寻附近的蓝牙外围设备..\n" + res.errMsg,
        });
        //监听寻找到新设备的事件

        self.onBluetoothDeviceFound()
      },
      fail: function(res) {},
      complete: function(res) {},
    })
  },

  //监听寻找到新设备的事件
  onBluetoothDeviceFound() {
    wx.onBluetoothDeviceFound((res) => {

      //遍历设备
      res.devices.forEach(device => {
        //排除设备名称不一致的情况
        if (!device.name && !device.localName) {
          return
        }
        console.log("------监听寻找到新设备的事件------\n", res)
        this.setData({
          debugM: "寻找到新设备\n" + res.devices[0].name,
        });
        const foundDevices = this.data.bluetoothDevices
        const idx = inArray(foundDevices, 'deviceId', device.deviceId)//判断当前设备是否列表中
        const data = {}
        //当前设备不在已有列表中，向列表中添加当前设备
        if (idx === -1) {
          data[`bluetoothDevices[${foundDevices.length}]`] = device
        } else {//当前设备在已有列表中，向列表中更新当前设备
          data[`bluetoothDevices[${idx}]`] = device
        }
        this.setData(data)
      })
    })
  },

  stopBluetoothDevicesDiscovery:function(e){
    wx.stopBluetoothDevicesDiscovery({
      success: (res) =>{
        console.log("------停止搜寻附近的蓝牙外围设备------\n", res)
        this.setData({
          debugM: "停止搜寻附近的蓝牙外围设备\n"+res.errMsg
        });
      },
    })
  },

  closeBluetoothAdapter() {
    wx.closeBluetoothAdapter({
      success: (res) =>{
        console.log("------关闭蓝牙服务------\n", res)
        this.setData({
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        });
      },
    })
    this._discoveryStarted = false
  },

  createBLEConnection:function(e) {
    const ds = e.currentTarget.dataset  //获取当前蓝牙目标
    const deviceId = ds.deviceId        //获取蓝牙设备ID
    const name = ds.name                //获取蓝牙名称
    //创建连接
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        console.log("------连接低功耗蓝牙设备------\n", res)
        this.setData({
          connected: true,
          name,
          deviceId,
          debugM: "关闭蓝牙服务\n" + res.errMsg,
        })
        //获取蓝牙设备服务
        this.getBLEDeviceServices(deviceId)
      }
    })
    this.stopBluetoothDevicesDiscovery()
  },

  getBLEDeviceServices(deviceId) {
    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        console.log(deviceId)
        console.log("------获取蓝牙设备(" + deviceId +")所有服务------\n", res)
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

  writeBLECharacteristicValue() {
    // 向蓝牙设备发送一个0x00的16进制数据
    let buffer = new ArrayBuffer(1)
    let dataView = new DataView(buffer)
    dataView.setUint8(0, Math.random() * 255 | 0)
    wx.writeBLECharacteristicValue({
      deviceId: this._deviceId,
      serviceId: this._deviceId,
      characteristicId: this._characteristicId,
      value: buffer,
    })
  },

  closeBLEConnection() {
    wx.closeBLEConnection({
      deviceId: this.data.deviceId
    })
    this.setData({
      connected: false,
      chs: [],
      canWrite: false,
    })
  },
})