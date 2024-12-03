# DonutQMLPlayer
本项目用于练习自封装的音视频播放器SDK，小型OpenGL渲染引擎，以及QML与C++联合编程

1.GUI部分使用QML开发，

2.音视频播放器SDK参考ffplay的处理逻辑，使用C++11进行封装与重构，实现了ffplay的大部分功能，并通过SoundTouch库实现音频的倍速不变调的处理由于是使用C++进行面向对象的封装，理论上可以移植到安卓平台(音频部分已经使用OpenSLES替换本项目的SDL，视频渲染部分使用OpenGLES替换)
  
3.使用自封装的OpenGL引擎(参考油管大佬cherno的Hazel引擎，对其进行简化并使用QOpenGLFunctions替换)，能实现简单的平移旋转缩放操作

以下是效果图
![image](https://github.com/user-attachments/assets/7a3a1a04-9893-4499-9a89-f0f096bdfe30)
![image](https://github.com/user-attachments/assets/03b50be7-66ec-4cee-95a3-2ec1a71feb50)
![image](https://github.com/user-attachments/assets/4095ad86-c7a1-4804-8004-5e564dcafff6)
![image](https://github.com/user-attachments/assets/9d032f9e-fcec-4d06-940e-51af9e9142bc)
