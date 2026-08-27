#include <Arduino.h>
#include "app/car_app.hpp"

// 全局对象保持单例
car::CarApp app;

void setup() {
    // arduino入口转交给应用层，硬件网络和ros初始化都在CarApp中集中管理
    app.setup();
}

void loop() {
    // loop内部是协作式调度，任何模块都不能长时间阻塞
    app.loop();
}