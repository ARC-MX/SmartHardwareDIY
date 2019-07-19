// pages/bleinfo/bleinfo.js


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
    actionSheetHidden: false,
    filterEnableChecked: false,
    debugM: "请获取蓝牙设备",
    discoveryFlag: true,
    hasBluetoothDivices: false,
    connected: false,
    chs: [],
    bluetoothDevices: []
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var self = this; //转存this对象
    //搜索蓝牙设备   包含初始化蓝牙
    wx.openBluetoothAdapter({ //初始化蓝牙模块
      //初始化成功，开始搜寻附近的蓝牙外围设备
      success: function (res) {
        console.log("------初始化蓝牙模块成功------\n", res)
        self.startBluetoothDevicesDiscovery()
        self.setData({
          debugM: "初始化蓝牙模块成功\n" + res.errMsg,
          discoveryFlag: false
        });
      },
      //初始化失败，如果失败原因是 “当前蓝牙适配器不可用”那么监听蓝牙状态是否改变
      //当蓝牙适配器是可用时再次开始搜寻附近的蓝牙外围设备
      fail: function (res) {
        console.log("------初始化蓝牙模块故障，尝试解决------\n", res)
        self.setData({
          debugM: "初始化蓝牙模块故障，尝试解决\n" + res.errMsg
        });
        if (res.errCode === 10001) {
          wx.onBluetoothAdapterStateChange(function (res) {
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
      complete: function (res) {
        console.log("------初始化蓝牙模块结束------\n", res)
      },
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
   * 设定长按监听事件--监听长按界面动作
   */
  toggleActionSheet: function () {
    console.info("长按触发");
    this.setData({
      actionSheetHidden: !this.data.actionSheetHidden
    });
  },
  /**
   * 设定主服务过滤，只显示含有主服务的BLE设备
   */
  filterEnable:function(){
    this.setData({
      filterEnableChecked: !this.data.filterEnableChecked,

    })
    console.log(this.data.filterEnableChecked ? "设定蓝牙主服务过滤使能": "设定蓝牙主服务过滤关闭")
    if (this.data.filterEnableChecked){
      this.setData({
        bluetoothDevices: []
      })
    }
  },

  //
  startBluetoothDevicesDiscovery: function () {
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
      success: function (res) {
        console.log("------开始搜寻附近的蓝牙外围设备..------\n", res)
        self.setData({
          debugM: "开始搜寻附近的蓝牙外围设备..\n" + res.errMsg,
        });
        //监听寻找到新设备的事件

        self.onBluetoothDeviceFound()
      },
      fail: function (res) { },
      complete: function (res) { },
    })
  },

  //监听寻找到新设备的事件
  onBluetoothDeviceFound:function() {
    wx.onBluetoothDeviceFound((res) => {
      
        res.devices.forEach(device => {
          this.filterEachDevice(device)
        })
    })
  },

  //遍历设备
  filterEachDevice: function (device){
      //排除设备名称不一致的情况
      if (this.data.filterEnableChecked) {
        console.log("------监听寻找到新设备并过滤------\n")
        if (!device.name && !device.localName) {
          return;
        }
      } else {
        
        this.setData({
          debugM: "寻找到新设备\n" + device.name,
        });
        //console.log("------监听寻找到新设备的事件------\n",device.advertisServiceUUIDs.length)
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
      }
 },
  createBLEConnectionPage:function(e){

//    const ds = e.currentTarget.dataset  //获取当前蓝牙目标
//    const deviceId = ds.deviceId        //获取蓝牙设备ID
//    const name = ds.name                //获取蓝牙名称

    //拿到点击的index下标
    
    var index = e.currentTarget.dataset.index
    //将对象转为string
    var bluetoothDevice = JSON.stringify(this.data.bluetoothDevices[index])
    wx.navigateTo({
      url: '../connection/connection?currentTarget=' + bluetoothDevice
    })
  }


})

