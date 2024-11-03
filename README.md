## little-Emma
### 基于GD32F303和MAX30102的简易脉搏血氧检测仪器
### Author: @kkl

> 2023年深圳大学电子设计校赛一等奖作品归档。

---

硬件开源地址：https://oshwhub.com/hugego/Little-Emma

## Snapshots
![](/3.Pictures/image-0.jpg)
![](/3.Pictures/image-1.jpg)
![](/3.Pictures/image-2.jpg)

## Introduction

**Little-Emma**是一款可便携、多功能、优雅的简易反射式脉搏血氧仪。

该作品主控采用`GD32F303CCT6`，脉搏血氧数据检测模块采用`MAX30102`，OLED搭载`HugoUI`，流畅显示。

> Plus：界面灵感来源于稚晖君制作的UltraLink.

## Attributes

- **流畅优雅的界面显示：** 更加优秀丝滑的人机交互体验。
- **使用MAX30102高精度血氧数据采集模块：** 搭配多次优化的算法，在追求精度的同时保持稳定性。
- **板载旋转编码器：** 操作更加方便快捷迅速。
- **板载Flash：** 使用`Fatfs`文件系统，保存用户数据。
- **引出蓝牙串口接口：** 可通过蓝牙连接上位机，显示血氧波形。

## Showcase
<div align="center">  
  <img src="/3.Pictures/video-0.gif" alt="Showcase" />  
</div>  
