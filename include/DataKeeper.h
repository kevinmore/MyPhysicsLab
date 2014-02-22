//This class is used to store data
#include <vector>
#include <string>

#pragma once

using namespace std;
class DataKeeper
{
public:
	DataKeeper(void);
	~DataKeeper(void);

	void setData(string type, float data);
	vector<float> getData(string type);
	void clearData(void);

protected:
	//a bunch of data vectors
	vector<float> Time;
	vector<float> CenterOfMassInWorld_X;
	vector<float> CenterOfMassInWorld_Y;
	vector<float> CenterOfMassInWorld_Z;
	vector<float> LinearVelocity_X;
	vector<float> LinearVelocity_Y;
	vector<float> LinearVelocity_Z;
	vector<float> ResultantLinearVelocity;
	vector<float> KineticEnergy;
	vector<float> LinearImpulse_X;
	vector<float> LinearImpulse_Y;
	vector<float> LinearImpulse_Z;
	vector<float> ResultantLinearImpulse;
	vector<float> AngularVelocity_X;
	vector<float> AngularVelocity_Y;
	vector<float> AngularVelocity_Z;
	vector<float> ResultantAngularVelocity;
	vector<float> GravPotentialEnergy;
	vector<float> TotalEnergy;

};

