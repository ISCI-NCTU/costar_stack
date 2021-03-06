//
//  symmetricOrientationRealignment.h
//  
//
//  Created by Felix Jo on 1/27/16.
//
//

#ifndef symmetricOrientationRealignment_h
#define symmetricOrientationRealignment_h

// ros stuff
#include <ros/ros.h>
#include <Eigen/Geometry>
#include "sp_segmenter/utility/typedef.h"
#include "sp_segmenter/utility/utility.h"
#include <math.h>
#include <boost/math/constants/constants.hpp>

const double degToRad = boost::math::constants::pi<double>() / 180.0;

//for normalizing object rotation
struct objectSymmetry
{

    double roll;
    double pitch;
    double yaw;

    // for setting preferred axis
    std::string preferred_axis;
    double preferred_step;

    // NOTE: input is in degrees
    objectSymmetry(const double &inputRoll, const double &inputPitch, const double &inputYaw)
        : roll(inputRoll*degToRad), pitch(inputPitch*degToRad), yaw(inputYaw * degToRad),
          preferred_axis(""),
          preferred_step(0.0) {}

    // NOTE: input is in degrees
    objectSymmetry(const double &inputRoll, const double &inputPitch, const double &inputYaw,
        const std::string &axis, const double& step)
        : roll(inputRoll*degToRad), pitch(inputPitch*degToRad), yaw(inputYaw * degToRad),
          preferred_axis(axis),
          preferred_step(step*degToRad) {}

    objectSymmetry() : roll(0.), pitch(0.), yaw(0.), preferred_axis(""), preferred_step(0.) {}
};

// Pass current ROS node handle
// Load in parameters for objects from ROS namespace
std::map<std::string, objectSymmetry> fillDictionary(const ros::NodeHandle &nh, const std::vector<std::string> &cur_name)
{
    std::map<std::string, objectSymmetry> objectDict;
    std::cerr << "LOADING IN OBJECTS\n";
    for (unsigned int i = 0; i < cur_name.size(); i++) {
        std::cerr << "Name of obj: " << cur_name[i] << "\n";

        double r, p, y, step;
        std::string preferred_axis;
        nh.param(cur_name.at(i)+"/x", r, 360.0);
        nh.param(cur_name.at(i)+"/y", p, 360.0);
        nh.param(cur_name.at(i)+"/z", y, 360.0);
        nh.param(cur_name.at(i)+"/preferred_step", step, 360.0);
        nh.param(cur_name.at(i)+"/preferred_axis", preferred_axis, std::string("z"));

        objectDict[cur_name.at(i)] = objectSymmetry(r, p, y, preferred_axis, step);
    }
    return objectDict;
}


template <typename numericStandard>
void realignOrientation (Eigen::Matrix<numericStandard, 3, 3> &rotMatrix, const objectSymmetry &object, const int axisToAlign, const bool withRotateSpecificAxis = false, const int rotateAroundSpecificAxis = 0)
{
    Eigen::Vector3f objAxes[3];
    objAxes[0] = Eigen::Vector3f(rotMatrix(0,0),rotMatrix(1,0),rotMatrix(2,0));
    objAxes[1] = Eigen::Vector3f(rotMatrix(0,1),rotMatrix(1,1),rotMatrix(2,1));
    objAxes[2] = Eigen::Vector3f(rotMatrix(0,2),rotMatrix(1,2),rotMatrix(2,2));
    double pi = boost::math::constants::pi<double>();
    Eigen::Vector3f axis(0,0,0);
    axis[axisToAlign] = 1;

    double dotProduct = axis.dot(objAxes[axisToAlign]);
    double angle = std::acos(dotProduct); //since the vector is unit vector
    
    Eigen::Vector3f bestAxis[3];
    bestAxis[0] = Eigen::Vector3f(1,0,0);
    bestAxis[1] = Eigen::Vector3f(0,1,0);
    bestAxis[2] = Eigen::Vector3f(0,0,1);

    int axisToRotate = 0;
    double objectLimit = std::numeric_limits<double>::max();
    
    if (!withRotateSpecificAxis)
    {
         // pick smallest object Limit that correspond to the object symmetry
        for (int i = 1; i < 3; i++) {
            double tmp;
            // set the rotation step that correspond to object symmetry
            switch ((i + axisToAlign)%3) {
                case 0:
                    tmp = object.roll;
                    break;
                case 1:
                    tmp = object.pitch;
                    break;
                default:
                    tmp = object.yaw;
                    break;
            }
            if (tmp < objectLimit){
                objectLimit = tmp;
                axisToRotate = (i + axisToAlign)%3;
            }
        }
    }
    else
    {
        axisToRotate = rotateAroundSpecificAxis;
        double tmp;
        switch (axisToRotate) {
            case 0:
                tmp = object.roll;
                break;
            case 1:
                tmp = object.pitch;
                break;
            default:
                tmp = object.yaw;
                break;
        }
        objectLimit = tmp;
    }

    //    std::cerr << "Number of step: " << std::floor(abs(angle)/objectLimit + 0.3333) << " " << angle * 180 / pi << " " << objectLimit * 180 / pi <<std::endl;
    
    if (std::floor(std::abs(angle+0.5236)/objectLimit) < 1) {
        //min angle = within 30 degree to the objectLimit to be aligned
        std::cerr << "Angle: " << angle * 180 / pi << " is too small, no need to fix the rotation\n";
        return;
    }
    std::cerr << "Initial matrix\n" << rotMatrix << std::endl;

    // rotate target axis to align as close as possible with its original axis by rotating the best axis to align the target axis. Use -angle because we want to undo the rotation
    rotMatrix = rotMatrix * Eigen::AngleAxisf(std::round(angle/objectLimit) * objectLimit,bestAxis[axisToRotate]);
    std::cerr << "Corrected matrix\n" << rotMatrix << std::endl;
    std::cerr << "Rotate axis" << axisToRotate << " : " << angle * 180 / pi << " -> " << std::round(angle/objectLimit) * objectLimit * 180 / pi <<std::endl;
}

template <typename numericStandard>
Eigen::Matrix<numericStandard, 3, 1> extractRPYfromRotMatrix(const Eigen::Matrix<numericStandard, 3, 3> &input, bool reversePitch = false)
{
    Eigen::Matrix<numericStandard, 3, 1> result;
    numericStandard x_component = std::sqrt(input(0,0) * input(0,0) + input(1,0) *input(1,0));
    // since using sqrt, there are 2 solution for x_component
    x_component = reversePitch ? -x_component : x_component;
    result[1] = std::atan2(-input(2,0), x_component);
    result[2] = std::atan2(input(1,0)/std::cos(result[1]),input(0,0)/std::cos(result[1]));
    result[0] = std::atan2(input(2,1)/std::cos(result[1]),input(2,2)/std::cos(result[1]));
    return result;
}

// Compute the best matching orientation for an object with multiple symmetries.
// This implementation works by computing quaternion angular distance between
// identity and all candidate orientations, generated by rotating around the 
// each axis with a step specified in the objectSymmetry struct.
// For example:
// A cube might have 90 degree steps around x, y, and z.
template <typename numericType>
Eigen::Quaternion<numericType> normalizeModelOrientation(const Eigen::Quaternion<numericType> &q_from_pose, const objectSymmetry &object)
{
    static const bool debug = false;
    const double pi = boost::math::constants::pi<double>();
    Eigen::Matrix3f symmetricOffset;
    Eigen::Quaternion<numericType> minQuaternion;
    unsigned int num_roll_steps = std::ceil(2*pi / object.roll);
    unsigned int num_pitch_steps = std::ceil(2*pi / object.pitch);
    unsigned int num_yaw_steps = std::ceil(2*pi / object.yaw);
    if (debug) {
        std::cout << ">>> " << num_roll_steps << ", " << 
         num_yaw_steps << ", " << 
         num_pitch_steps << "\n";
        std::cout << "=== " << object.roll << ", " <<
            object.pitch << ", " <<
            object.yaw << "\n";
    }
    double minAngle = std::numeric_limits<double>::max();
    for (unsigned int i = 0; i < num_roll_steps; i++)
    {
        for (unsigned int j = 0; j < num_pitch_steps; j++)
        {
            for (unsigned int k = 0; k < num_yaw_steps; k++) 
            {
                symmetricOffset =  Eigen::Matrix3f::Identity()
                    * Eigen::AngleAxisf(i * object.yaw, Eigen::Vector3f::UnitZ())
                    * Eigen::AngleAxisf(j * object.pitch, Eigen::Vector3f::UnitY())
                    * Eigen::AngleAxisf(k * object.roll, Eigen::Vector3f::UnitX());
                Eigen::Quaternion<numericType> rotatedInputQuaternion = q_from_pose * Eigen::Quaternion<numericType>(symmetricOffset);
                //std::cerr << " - "
                //    << minAngle << " " << rotatedInputQuaternion.angularDistance(Eigen::Quaternion<numericType>::Identity()) 
                //    << "\n";
                if (minAngle > rotatedInputQuaternion.angularDistance(Eigen::Quaternion<numericType>::Identity())) 
                {
                    minAngle = rotatedInputQuaternion.angularDistance(Eigen::Quaternion<numericType>::Identity ());
                    minQuaternion = rotatedInputQuaternion;
                    if (debug) {
                        std::cout << "Found better matrix with angular distance: " << minAngle * 180 / pi << std::endl;
                    }
                }
            }
        }
    }
    if (debug) {
        std::cout << "\nBest rotation matrix: \n" << minQuaternion.matrix() << std::endl;
        std::cout << "Best angular Distance:"
            << minQuaternion.angularDistance(Eigen::Quaternion<numericType>::Identity())
            << std::endl;
    }

    Eigen::Matrix<numericType,3,3> normalize_orientation = minQuaternion.matrix();
    realignOrientation(normalize_orientation,object,2);
    realignOrientation(normalize_orientation,object,0,true,2); 

    return Eigen::Quaternion<numericType>(normalize_orientation);
}

template <typename numericType>
void printQuaternion(const Eigen::Quaternion<numericType> &input)
{
  std::cerr << "Q: " << input.w() << ", " << input.x() << ", " << input.y() << ", " << input.z() << std::endl;
}

template <typename numericType>
Eigen::Quaternion<numericType> normalizeModelOrientation(const Eigen::Quaternion<numericType> &q_new, const Eigen::Quaternion<numericType>  &q_previous, const objectSymmetry &object)
{
  std::cerr << "Input Qnew: "; printQuaternion(q_new);
  std::cerr << "Input Qold: "; printQuaternion(q_previous);
  Eigen::Quaternion<numericType> rotationChange = q_previous.inverse() * q_new;
  // Since the rotationChange should be close to identity, realign the rotationChange as close as identity based on symmetric property of the object
  rotationChange = normalizeModelOrientation(rotationChange, object);
  
  std::cerr << "Output Q: "; printQuaternion(q_previous * rotationChange);
  // fix the orientation of new pose
  return (q_previous * rotationChange);
}

template <typename numericType>
Eigen::Quaternion<numericType> normalizeModelOrientation(const poseT &newPose,const poseT &previousPose, const objectSymmetry &object)
{
    return (normalizeModelOrientation(newPose.rotation,previousPose.rotation,object));
}


template <typename numericType>
void normalizeAllModelOrientation (std::vector<poseT> &all_poses, const Eigen::Quaternion<numericType> &normalOrientation, const std::map<std::string, objectSymmetry> &objectDict)
{
  for (unsigned int i = 0; i < all_poses.size(); i++)
  {
    // std::cerr << "Rot"<< i <<" before normalization: \n" << all_poses[i].rotation.toRotationMatrix() << std::endl;
    std::cerr << "Object name: " << all_poses[i].model_name << std::endl;
    all_poses[i].rotation = normalizeModelOrientation(all_poses[i].rotation, normalOrientation, objectDict.find(all_poses[i].model_name)->second);
    // std::cerr << "Rot"<< i <<" after normalization: \n" << all_poses[i].rotation.toRotationMatrix() << std::endl;
  }
}

#endif /* symmetricOrientationRealignment_h */
