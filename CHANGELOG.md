# DouniuServer - Changelog

## Version 1.2 - 2016/09/12
- 完善命令封装格式，包括tryingbanker/stake/play。
- 定义宏USE_IN_ANDROID判断是否Android版本。

## Version 1.1 - 2016/09/05
- 通过多线程、Socket等实现基本的游戏逻辑框架，实现login/logout/prepare等命令的收发。
- 另外，考虑到本地网络情况（服务器不方便连外网），计划架设局域网，将服务器和客户端放置同一局域网。故需要为andoird jni做准备。

## Version 1.0 - 2016/07/29
- Initial version about server implementation of game application. 
