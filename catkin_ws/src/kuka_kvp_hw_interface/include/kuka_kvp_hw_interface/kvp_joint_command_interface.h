/**
Copyright 2017 Ivar Eriksen

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
language governing permissions and limitations under the License.
*/

/* Author: Ivar Eriksen
   Desc: Control a KUKA robot using KUKAVARPROXY (KVP).
         Inspired from davetcoleman/ros_control_boilerplate and kuka_experimental/kuka_rsi_hw_interface
*/

#ifndef KVP_JOINT_COMMAND_INTERFACE_H
#define KVP_JOINT_COMMAND_INTERFACE_H

// C++
#include <string>
#include <vector>

// Boost
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>

// ROS
#include <std_msgs/String.h>
#include <ros/ros.h>
#include <realtime_tools/realtime_publisher.h>

// ROS control
#include <controller_manager/controller_manager.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/robot_hw.h>

// ROS industrial
#include <industrial_utils/param_utils.h>

// KVP communication
#include <BoostClientCross.h>

namespace kuka_kvp_hw_interface
{
class KVPJointCommandInterface : public hardware_interface::RobotHW
{
protected:
  ros::NodeHandle nh_;

  // Joint states
  std::vector<double> joint_position_;
  std::vector<double> joint_velocity_;
  std::vector<double> joint_effort_;

  // Joint commands
  std::vector<double> joint_position_command_;
  std::vector<double> joint_velocity_command_;
  std::vector<double> joint_effort_command_;

  // Hardware interfaces
  hardware_interface::JointStateInterface joint_state_interface_;
  hardware_interface::PositionJointInterface position_joint_interface_;

  // Configuration variables
  std::vector<std::string> joint_names_;
  std::size_t num_joints_;

  // Variables relating to KVP configuration
  std::string ip_;
  std::string kvp_read_from_;
  std::string kvp_write_to_;

private:
  // Variables for robot communication
  BoostClientCross robot_read_;
  BoostClientCross robot_write_;
  boost::barrier robot_update_start_;
  boost::barrier robot_update_done_;
  boost::thread read_thread_;
  boost::thread write_thread_;
  bool robot_run_ = true;
  std::unique_ptr<realtime_tools::RealtimePublisher<std_msgs::String>> realtime_pub_;

  /** \brief Read state from robot */
  void readKVP();

  /** \brief Write state to robot */
  void writeKVP();

public:
  KVPJointCommandInterface(ros::NodeHandle& nh);

  /** \brief Read state from robot
   *   Releases both read and write threads
   */
  void read();

  /** \brief Connects to robot and starts worker threads */
  void connect();

  /** \brief Write to robot
   *Both read and write threads are released from read() function
   */
  void write(){};

  /** \brief Destructor. Calls disconnect */
  ~KVPJointCommandInterface()
  {
    disconnect();
  };

  /** \brief Stop threads and disconnect from robot */
  void disconnect();

};  // class

}  // namespace

#endif
