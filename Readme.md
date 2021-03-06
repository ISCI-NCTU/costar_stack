# CoSTAR 

***Collaborative System for Task Automation and Recognition***

Our goal is to build a system which facilitates end-user instruction of robots to perform a a variety of different tasks. CoSTAR allows users to program robots to perform complex tasks such as sorting, assembly, and more. Tasks are represented as Behavior Trees.

[![Collaborative Assembly Example](https://img.youtube.com/vi/QS0cOPJFIDg/0.jpg)](https://youtu.be/QS0cOPJFIDg)

For more videos you can check out the [CoSTAR YouTube Channel](https://www.youtube.com/playlist?list=PLF86ez-NVmyEDgpmwpnpM6LyNwtkiWxAf).

These are the tools and utilities we created to get the CoSTAR project up and off the ground. This document describes the CoSTAR project for an LBR iiwa 14 R820 with an attahed Robotiq 3-finger adaptive gripper.

To fully take advantage of these capabilities you will need:

  - sp_segmenter: object detection and pose estimation library
  - instructor: our custom user interface
  - a KUKA LBR iiwa or Universal Robots UR5
  - a Robotiq 3-finger gripper or 2-finger gripper

This is a project by members of the JHU Laboratory for Computational Sensing and Robotics, namely Chris Paxton, Kel Guerin, Andrew Hundt, and Felix Jonathan. If you find this code useful, please cite:
```
@article{paxton2016costar,
  title={CoSTAR: Instructing Collaborative Robots with Behavior Trees and Vision},
  author={Paxton, Chris and Hundt, Andrew and Jonathan, Felix and Guerin, Kelleher and Hager, Gregory D},
  journal={arXiv preprint arXiv:1611.06145},
  year={2016}
}
```

## Installation

Check out installation instructions [here](INSTALL.md).

## Starting CoSTAR for the LBR iiwa with GRL

Making sure you are connected to the robot:

```
ping 192.170.10.2 # connection to FRI port for joint states
ping 172.31.1.146 # connection to Robotiq 3-finger gripper to send commands
ping 172.31.1.147 # connection to JAVA port on the robot to send commands
```

How to bring this robot up on our own platform:

```
# core features and UI
roslaunch costar_bringup iiwa14_s_model.launch

# object detection and moveit planning scene
roslaunch sp_segmenter SPServerStructureAssembly.launch
```

You also need to launch the GRL KUKA ROS Driver. GRL is the [Generic Robot Library](https://github.com/ahundt/grl), which provides a low-level control interface for the KUKA LBR.

In particular, `costar_bringup` will launch the robot command driver, the gripper command services, the MoveIt services, Predicator, and Librarian.

To bring up the tool attachments run:

```
roslaunch ready_air stomper_tool.launch
```


## Starting CoSTAR in a Simulation

You can test CoSTAR in a simulation now. The basic launch procedure for a simulation is to run these commands:

```
roslaunch iiwa_gazebo iiwa_gazebo.launch trajectory:=false
roslaunch costar_bringup iiwa14_s_model.launch sim:=true start_sim:=false
```

You can then use the Instructor UI to move the robot around in simulation. If you want to bring up Gazebo and all the drivers at once instead of separately, launch as:

```
roslaunch costar_bringup iiwa14_s_model.launch sim:=true start_sim:=true
```

## CoSTAR Packages

  * [Bringup](costar_bringup/Readme.md): launch files, RVIZ configurations, et cetera
  * [Instructor](costar_instructor/Readme.md): user interface
  * [Librarian](costar_librarian/Readme.md): file management
  * [Predicator](costar_predicator/Readme.md): robot knowledge management
  * [Perception](costar_perception/Readme.md): semantic segmentation and object detection via [SP Segmenter](https://github.com/jhu-lcsr/sp_segmenter)
  * [Robot](costar_robot/Readme.md): utilities and services allowing high-level control of the robot and integrating these behaviors into the UI. Contains the `CostarArm` component.
  * Gripper: utilities for integrating different grippers into UI
  * Tools: packages used for data collection, maintaining MoveIt planning scene, and other purposes

## Contact

CoSTAR is maintained by Chris Paxton (cpaxton@jhu.edu).

Other core contributors include:
  * Felix Jonathan
  * Andrew Hundt
