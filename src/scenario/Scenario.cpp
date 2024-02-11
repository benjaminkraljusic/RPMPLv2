//
// Created by dinko on 17.02.22.
// Modified by nermin on 09.02.24.
//

#include "Scenario.h"
#include "RealVectorSpaceFCL.h"
#include "RealVectorSpaceState.h"
#include "Planar2DOF.h"
#include "Planar10DOF.h"
#include "xArm6.h"
#include "ConfigurationReader.h"

scenario::Scenario::Scenario(const std::string &config_file_path, const std::string &root_path)
{
    try
    {
        YAML::Node node = YAML::LoadFile(root_path + config_file_path);
        env = std::make_shared<env::Environment>(config_file_path, root_path);

        YAML::Node robot_node = node["robot"];
        std::string type = robot_node["type"].as<std::string>();
        int num_DOFs = robot_node["num_DOFs"].as<int>();
        if (type == "xarm6")
            robot = std::make_shared<robots::xArm6>(root_path + robot_node["urdf"].as<std::string>(),
                                                    robot_node["gripper_length"].as<float>(),
                                                    robot_node["table_included"].as<bool>());
        else if (type == "planar_2DOF")
            robot = std::make_shared<robots::Planar2DOF>(root_path + robot_node["urdf"].as<std::string>());
        else if (type == "planar_10DOF")
            robot = std::make_shared<robots::Planar10DOF>(root_path + robot_node["urdf"].as<std::string>());

        YAML::Node capsules_radius_node = robot_node["capsules_radius"];
        if (capsules_radius_node.IsDefined())
        {
            if (capsules_radius_node.size() != num_DOFs)
                throw std::logic_error("Number of capsules is not correct!");
                
            std::vector<float> capsules_radius;
            for (int i = 0; i < capsules_radius_node.size(); i++)
                capsules_radius.emplace_back(capsules_radius_node[i].as<float>());

            robot->setCapsulesRadius(capsules_radius);
        }

        YAML::Node max_vel_node = robot_node["max_vel"];
        if (max_vel_node.IsDefined())
        {
            if (max_vel_node.size() != num_DOFs)
                throw std::logic_error("The size of 'max_vel' is not correct!");

            std::vector<float> max_vel;
            for (int i = 0; i < max_vel_node.size(); i++)
                max_vel.emplace_back(max_vel_node[i].as<float>());

            robot->setMaxVel(max_vel);
        }

        YAML::Node max_acc_node = robot_node["max_acc"];
        if (max_acc_node.IsDefined())
        {
            if (max_acc_node.size() != num_DOFs)
                throw std::logic_error("The size of 'max_acc' is not correct!");

            std::vector<float> max_acc;
            for (int i = 0; i < max_acc_node.size(); i++)
                max_acc.emplace_back(max_acc_node[i].as<float>());

            robot->setMaxAcc(max_acc);
        }

        YAML::Node max_jerk_node = robot_node["max_jerk"];
        if (max_jerk_node.IsDefined())
        {
            if (max_jerk_node.size() != num_DOFs)
                throw std::logic_error("The size of 'max_jerk' is not correct!");

            std::vector<float> max_jerk;
            for (int i = 0; i < max_jerk_node.size(); i++)
                max_jerk.emplace_back(max_jerk_node[i].as<float>());
                
            robot->setMaxJerk(max_jerk);
        }

        std::string space_state = robot_node["space"].as<std::string>();
        if (space_state == "RealVectorSpace")
            ss = std::make_shared<base::RealVectorSpace>(num_DOFs, robot, env);
        else if (space_state == "RealVectorSpaceFCL")
            ss = std::make_shared<base::RealVectorSpaceFCL>(num_DOFs, robot, env);

        YAML::Node q_start_node = robot_node["q_start"];
        YAML::Node q_goal_node = robot_node["q_goal"];
        Eigen::VectorXf q_start_vec(num_DOFs);
        Eigen::VectorXf q_goal_vec(num_DOFs);
        if (q_start_node.size() != q_goal_node.size())
            throw std::logic_error("Start and goal size mismatch!");
        
        for (int i = 0; i < q_start_node.size(); i++)
        {
            q_start_vec(i) = q_start_node[i].as<float>();
            q_goal_vec(i) = q_goal_node[i].as<float>();
        }
        q_start = std::make_shared<base::RealVectorSpaceState>(q_start_vec);
        q_goal = std::make_shared<base::RealVectorSpaceState>(q_goal_vec);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
}
