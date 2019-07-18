// pages/bleinfo/bleinfo.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    actionSheetHidden: false,
    activeNames: ['1'],
    filterEnableChecked:false,

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {


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

    this.setData({
      show: !this.data.show
    });
    console.info("下拉事件");
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onPageScroll: function () {

    console.info("上拉事件");
    this.setData({

      actionSheetHidden: !this.data.actionSheetHidden
    });
  },
  toggleActionSheet() {
    this.setData({
      actionSheetHidden: !this.data.actionSheetHidden
    });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },

  collapseChange(event) {
    this.setData({
      activeNames: event.detail
    });
  }



})



/**

  < action - sheet hidden = "{{actionSheetShow}}" bindchange = "listenerActionSheet" >
    <action-sheet-item>
      <view class="content">内容</view>
      <switch bindchange="filterSwitchChange" />
    </action-sheet-item>
    <!--自动隐藏action - sheet-- >
      <action-sheet-cancel>取消</action-sheet-cancel>
</action - sheet >

 */