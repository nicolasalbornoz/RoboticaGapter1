#include <cstdlib>
#include <ros/ros.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/CommandInt.h>
#include <mavros_msgs/SetMode.h>
#include <geometry_msgs/PoseStamped.h>

int main(int argc, char **argv)
{
	int rate = 10;
	int i = 0;
	int aux = 100;

	ros::init(argc, argv, "mavros_takeoff");

	ros::NodeHandle n;
	ros::Rate r(rate);

	////////////////////////////////////////////
	/////////////////GUIDED/////////////////////
	////////////////////////////////////////////

	ros::ServiceClient cl = n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");

	mavros_msgs::SetMode srv_setMode;

	srv_setMode.request.base_mode = 0;
	srv_setMode.request.custom_mode = "GUIDED";

	if(cl.call(srv_setMode)){
		ROS_INFO("modo de vuelo enviado ok; %d", srv_setMode.response.success);
	}
	else{
		ROS_ERROR("Failed SetMode");
		return -1;
	}

	////////////////////////////////////////////
	///////////////////ARM//////////////////////
	////////////////////////////////////////////

	ros::ServiceClient arming_cl = n.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");

	mavros_msgs::CommandBool srv;

	srv.request.value = true;

	if(arming_cl.call(srv)){
		ROS_INFO("motores armados ok; %d", srv.response.success);
	}
	else{
		ROS_ERROR("Failed arming or disarming");
	}

	////////////////////////////////////////////
	/////////////////TAKEOFF////////////////////
	////////////////////////////////////////////

	ros::ServiceClient takeoff_cl = n.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/takeoff");

	mavros_msgs::CommandTOL srv_takeoff;

	srv_takeoff.request.altitude = 3;
	srv_takeoff.request.latitude = 0;
	srv_takeoff.request.longitude = 0;
	srv_takeoff.request.min_pitch = 0;
	srv_takeoff.request.yaw = 0;

	if(takeoff_cl.call(srv_takeoff)){
		ROS_INFO("despegue enviado ok; %d", srv_takeoff.response.success);
	}
	else{
		ROS_ERROR("Failed Takeoff");
	}

	sleep(10);

	////////////////////////////////////////////
	///////////primer vertice rombo/////////////
	////////////////////////////////////////////

	ros::Publisher local_pos_pub = n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local",10);

	geometry_msgs::PoseStamped pose;

	pose.pose.position.x = 3;
	pose.pose.position.y = 0;
	pose.pose.position.z = 3;

	ROS_INFO("Primer vertice");

	for(int i = aux; ros::ok() && i>0; --i){
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}

	ROS_INFO("X: %d", pose.pose.position.x);
	ROS_INFO("Y: %d", pose.pose.position.y);
	ROS_INFO("Z: %d", pose.pose.position.z);

	////////////////////////////////////////////
	////////////segundo vertice rombo///////////
	////////////////////////////////////////////

	pose.pose.position.x = 0;
	pose.pose.position.y = 1;
	pose.pose.position.z = 3;

	ROS_INFO("Segundo vertice");

	for(int i = aux; ros::ok() && i>0; --i)
	{
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}

	////////////////////////////////////////////
	////////////tercer vertice rombo////////////
	////////////////////////////////////////////

	pose.pose.position.x = -3;
	pose.pose.position.y = 0;
	pose.pose.position.z = 3;

	ROS_INFO("Tercer vertice");

	for(int i = aux; ros::ok() && i>0; --i)
	{
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}

	////////////////////////////////////////////
	///////////cuarto vertice rombo/////////////
	////////////////////////////////////////////

	pose.pose.position.x = 0;
	pose.pose.position.y = -1;
	pose.pose.position.z = 3;

	ROS_INFO("Cuarto vertice");

	for(int i = aux; ros::ok() && i>0; --i)
	{
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}

	////////////////////////////////////////////
	//////////primer vertice rombo//////////////
	////////////////////////////////////////////

	pose.pose.position.x = 3;
	pose.pose.position.y = 0;
	pose.pose.position.z = 3;

	ROS_INFO("Primer vertice");

	for(int i = aux; ros::ok() && i>0; --i)
	{
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}

	////////////////////////////////////////////
	/////////vuelta al punto inicial////////////
	////////////////////////////////////////////

	pose.pose.position.x = 0;
	pose.pose.position.y = 0;
	pose.pose.position.z = 3;

	ROS_INFO("Se prepara para aterrizar");

	for(int i = aux; ros::ok() && i>0; --i)
	{
		local_pos_pub.publish(pose);
		ros::spinOnce();
		r.sleep();
	}


	////////////////////////////////////////////
	///////////////////LAND/////////////////////
	////////////////////////////////////////////

	ros::ServiceClient land_cl = n.serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/land");

	mavros_msgs::CommandTOL srv_land;

	srv_land.request.altitude = 3;
	srv_land.request.latitude = 0;
	srv_land.request.longitude = 0;
	srv_land.request.min_pitch = 0;
	srv_land.request.yaw = 0;

	if(land_cl.call(srv_land))
	{
		ROS_INFO("land enviado ok %d", srv_land.response.success);
	}
	else
	{
		ROS_ERROR("Failed Land");
	}

return 0;
}

