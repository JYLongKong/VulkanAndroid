#include <jni.h>
#include <errno.h>
#include <vulkan/vulkan.h>
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include "help.h"
#include "MyVulkanManager.h"
#include "mylog.h"
#include "LightManager.h"
#include "CameraUtil.h"

extern "C"
{
int xPre;
int yPre;
float xDis;
float yDis;

/// Sample4_7
bool isClick = true;

/// Sample5_1
//float TOUCH_SCALE_FACTOR = 180.0f / 320;

/// Sample5_3
//float TOUCH_SCALE_FACTOR = 10.0f / 320; // 角度缩放比例

/// Sample5_9
float TOUCH_SCALE_FACTOR = 5.0f / 320;

/// Sample6_4
int pressType = 1;

/**
 * 事件处理回调方法
 * Sample4_1
 */
static int32_t engine_handle_input(struct android_app *app, AInputEvent *event) {
  //struct engine* engine = (struct engine*)app->userData;
  // 如果是MOTION事件(包含触屏和轨迹球)
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
    // 如果是触屏
    if (AInputEvent_getSource(event) == AINPUT_SOURCE_TOUCHSCREEN) {
      // 获取触控点坐标
      int x = AMotionEvent_getRawX(event, 0);
      int y = AMotionEvent_getRawY(event, 0);
      // 获取事件类型编号
      int32_t id = AMotionEvent_getAction(event);
      switch (id) {
        case AMOTION_EVENT_ACTION_DOWN: // 触摸点按下

          /// Sample4_1 ************************************************** start
//          MyVulkanManager::vpCenterX = x;
//          MyVulkanManager::vpCenterY = y;
//          // 控制边界
//          if (x > MyVulkanManager::screenWidth / 4 * 3)
//            MyVulkanManager::vpCenterX = MyVulkanManager::screenWidth / 4 * 3;
//          if (x < MyVulkanManager::screenWidth / 4)
//            MyVulkanManager::vpCenterX = MyVulkanManager::screenWidth / 4;
//          if (y > MyVulkanManager::screenHeight / 4 * 3)
//            MyVulkanManager::vpCenterY = MyVulkanManager::screenHeight / 4 * 3;
//          if (y < MyVulkanManager::screenHeight / 4)
//            MyVulkanManager::vpCenterY = MyVulkanManager::screenHeight / 4;
          /// Sample4_1 **************************************************** end

          /// Sample4_2 ************************************************** start
//          xPre = x;
//          yPre = y;
          /// Sample4_2 **************************************************** end

          /// Sample4_7、6_3 ********************************************** start
          isClick = true;
          xPre = x;
          yPre = y;
          /// Sample4_7、6_3 ************************************************ end

          break;
        case AMOTION_EVENT_ACTION_MOVE: // 触摸点移动

          /// Sample4_2、7_1 ********************************************* start
          xDis = x - xPre;                      // 计算触控点x位移
          yDis = y - yPre;                      // 计算触控点y位移
          MyVulkanManager::xAngle += yDis / 10; // 计算x轴旋转角
          MyVulkanManager::yAngle += xDis / 10; // 计算y轴旋转角
          xPre = x;                             // 记录触控点x坐标
          yPre = y;                             // 记录触控点y坐标
          /// Sample4_2、7_1 *********************************************** end

          /// Sample4_7、Sample6_4 *************************************** start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          if (abs((int) xDis) > 10 || abs((int) yDis) > 10) {
//            isClick = false;
//          }
          /// Sample4_7、Sample6_4 ***************************************** end

          /// Sample4_11 ************************************************* start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          if (abs((int) xDis) > 10 || abs((int) yDis) > 10) {
//            isClick = false;
//          }
//          if (!isClick) { // 若为滑动操作
//            MyVulkanManager::yAngle += xDis / 10; // 计算绕y轴转角
////            MyVulkanManager::zAngle += yDis / 10; // Sample4_12-计算绕z轴转角
//            MyVulkanManager::xAngle += yDis / 10; // Sample4_13-计算绕x轴转角
//            xPre = x;
//            yPre = y;
//          }
          /// Sample4_11 *************************************************** end

          /// Sample5_1 ************************************************** start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          MyVulkanManager::xAngle += yDis * TOUCH_SCALE_FACTOR;
//          MyVulkanManager::yAngle += xDis * TOUCH_SCALE_FACTOR;
//          xPre = x;
//          yPre = y;
          /// Sample5_1 **************************************************** end

          /// Sample5_3、Sample5_7 **************************************** start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          LightManager::lx = LightManager::lx + xDis * TOUCH_SCALE_FACTOR;  // 修改光源位置x点坐标
//          LightManager::ly = LightManager::ly - yDis * TOUCH_SCALE_FACTOR;  // 修改光源位置y点坐标
//          xPre = x;
//          yPre = y;
          /// Sample5_3、Sample5_7 ***************************************** end

          /// Sample5_6 ************************************************** start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          LightManager::ldx = LightManager::ldx + xDis * TOUCH_SCALE_FACTOR;
//          LightManager::ldy = LightManager::ldy - yDis * TOUCH_SCALE_FACTOR;
//          // 控制光照方向范围
//          if (LightManager::ldx <= -4.0f) {
//            LightManager::ldx = -4.0f;
//          }
//          if (LightManager::ldx >= 4.0f) {
//            LightManager::ldx = 4.0f;
//          }
//          if (LightManager::ldy <= -4.0f) {
//            LightManager::ldy = -4.0f;
//          }
//          if (LightManager::ldy >= 4.0f) {
//            LightManager::ldy = 4.0f;
//          }
//          xPre = x;
//          yPre = y;
          /// Sample5_6 **************************************************** end

          /// Sample6_1、6_3、6_5、6_7 ************************************ start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          MyVulkanManager::yAngle += xDis * 180.0 / 600;
//          MyVulkanManager::zAngle += yDis * 180.0 / 600;
//          xPre = x;
//          yPre = y;
//          if (abs((int) xDis) > 10 || abs((int) yDis) > 10) {
//            isClick = false;
//          }
          /// Sample6_1、6_3、6_5、6_7 ************************************** end

          /// Sample6_6 ************************************************** start
//          xDis = x - xPre;
//          yDis = y - yPre;
//          CameraUtil::calCamera(float(-yDis * 180 / 1000.0), 0); // 更新摄像机9参数
//          LightManager::move(float(xDis * 180 / 1000.0)); // 移动光源
//          xPre = x;
//          yPre = y;
          /// Sample6_6 **************************************************** end

          break;
        case AMOTION_EVENT_ACTION_UP:   // 触摸点抬起

          /// Sample4_7 ************************************************** start
//          if (isClick) {
//            MyVulkanManager::topologyWay = (++MyVulkanManager::topologyWay % ShaderQueueSuit_Common::topologyCount);
//          }
//          isClick = true;
          /// Sample4_7 **************************************************** end

          /// Sample4_11 ************************************************* start
//          if (isClick) {
//            MyVulkanManager::ViewPara = ++MyVulkanManager::ViewPara % 2;  // 更新当前采用的视角索引
//            MyVulkanManager::initMatrix();  // 重新初始化矩阵
//          }
          /// Sample4_11 *************************************************** end

          /// Sample4_12 ************************************************* start
//          if (isClick) {
//            MyVulkanManager::ProjectPara = ++MyVulkanManager::ProjectPara % 2;
//            MyVulkanManager::initMatrix();  // 重新初始化矩阵
//          }
          /// Sample4_12 *************************************************** end

          /// Sample4_13 ************************************************* start
//          if (isClick) {
//            MyVulkanManager::depthOffsetFlag = ++MyVulkanManager::depthOffsetFlag % 3;
//          }
          /// Sample4_13 *************************************************** end

          /// Sample6_3 ************************************************** start
//          if (isClick) {
//            if (x < MyVulkanManager::screenWidth / 2) { // 触控位置在屏幕左侧
//              MyVulkanManager::samplerType = (++MyVulkanManager::samplerType % 4);
//            } else {  // 触控位置在屏幕右侧
//              MyVulkanManager::texType = (++MyVulkanManager::texType % 3);
//            }
//          }
          /// Sample6_3 **************************************************** end

          /// Sample6_4 ************************************************** start
//          if (isClick) {
//            switch (pressType) {
//              case 0:MyVulkanManager::smallType = 0;
//                MyVulkanManager::bigType = 1;
//                break;
//              case 1:MyVulkanManager::smallType = 1;
//                MyVulkanManager::bigType = 0;
//                break;
//              case 2:MyVulkanManager::smallType = 0;
//                MyVulkanManager::bigType = 0;
//                break;
//              case 3:MyVulkanManager::smallType = 1;
//                MyVulkanManager::bigType = 1;
//                break;
//              default:break;
//            }
//            pressType = (++pressType % 4);
//          }
          /// Sample6_4 **************************************************** end

          break;
        default:break;
      }
    }
    return true;
  }
  return false;
}

//命令回调方法
static void engine_handle_cmd(struct android_app *app, int32_t cmd) {
  //struct engine* engine = (struct engine*)app->userData;
  switch (cmd) {
    case APP_CMD_SAVE_STATE://保存窗口事件
      LOGI("APP_CMD_SAVE_STATE");
      break;
    case APP_CMD_INIT_WINDOW://初始化窗口事件
      MyVulkanManager::doVulkan();
      LOGI("APP_CMD_INIT_WINDOW");
      break;
    case APP_CMD_TERM_WINDOW://终止窗口事件
      LOGI("APP_CMD_TERM_WINDOW");
      break;
    case APP_CMD_GAINED_FOCUS://获取焦点事件
      LOGI("APP_CMD_GAINED_FOCUS");
      break;
    case APP_CMD_LOST_FOCUS://失去焦点事件
      MyVulkanManager::loopDrawFlag = false;
      LOGI("APP_CMD_LOST_FOCUS");
      break;
  }
}

//入口函数
void android_main(struct android_app *app) {
  //这一句必须写
  app_dummy();
  MyVulkanManager::Android_application = app;
  MyData md;
  //设置应用的用户数据对象
  app->userData = &md;
  //设置应用的命令回调方法
  app->onAppCmd = engine_handle_cmd;
  //设置应用的事件处理回调方法
  app->onInputEvent = engine_handle_input;
  //将应用指针设置给MyData
  md.app = app;
  //标志位
  bool beginFlag = false;
  while (true) {
    int events;
    struct android_poll_source *source;
    //ALooper_pollAll函数的第一个参数为0 表示获取不到输入事件则立即返回
    //为-1表示获取不到就等待
    //循环获取事件消息并处理掉
    while ((ALooper_pollAll((beginFlag ? 0 : -1), NULL, &events, (void **) &source)) >= 0) {
      beginFlag = true;
      //处理事件
      if (source != NULL) {
        source->process(app, source);
      }
    }
    //做不断循环要做的工作，比如刷帧
  }
}

//extern "C"的右花括号
}