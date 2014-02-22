//This class is used to store data
#include "DataKeeper.h"

using namespace std;
DataKeeper::DataKeeper(void)
{
}


DataKeeper::~DataKeeper(void)
{
}

void DataKeeper::setData(string type, float data )
{
	if (type == "Time") Time.push_back(data);
	else if (type == "CenterOfMassInWorld_X") CenterOfMassInWorld_X.push_back(data);
	else if (type == "CenterOfMassInWorld_Y") CenterOfMassInWorld_Y.push_back(data);
	else if (type == "CenterOfMassInWorld_Z") CenterOfMassInWorld_Z.push_back(data);
	else if (type == "LinearVelocity_X") LinearVelocity_X.push_back(data);
	else if (type == "LinearVelocity_Y") LinearVelocity_Y.push_back(data);
	else if (type == "LinearVelocity_Z") LinearVelocity_Z.push_back(data);
	else if (type == "ResultantLinearVelocity") ResultantLinearVelocity.push_back(data);
	else if (type == "KineticEnergy") KineticEnergy.push_back(data);
	else if (type == "LinearImpulse_X") LinearImpulse_X.push_back(data);
	else if (type == "LinearImpulse_Y") LinearImpulse_Y.push_back(data);
	else if (type == "LinearImpulse_Z") LinearImpulse_Z.push_back(data);
	else if (type == "ResultantLinearImpulse") ResultantLinearImpulse.push_back(data);
	else if (type == "AngularVelocity_X") AngularVelocity_X.push_back(data);
	else if (type == "AngularVelocity_Y") AngularVelocity_Y.push_back(data);
	else if (type == "AngularVelocity_Z") AngularVelocity_Z.push_back(data);
	else if (type == "ResultantAngularVelocity") ResultantAngularVelocity.push_back(data);
	else if (type == "GravPotentialEnergy") GravPotentialEnergy.push_back(data);
	else if (type == "TotalEnergy") TotalEnergy.push_back(data);
	
}

vector<float> DataKeeper::getData( string type )
{
	//default data vector
	vector<float> empty;

	if (type == "Time") return Time;
	else if (type == "CenterOfMassInWorld_X") return CenterOfMassInWorld_X;
	else if (type == "CenterOfMassInWorld_Y") return CenterOfMassInWorld_Y;
	else if (type == "CenterOfMassInWorld_Z") return CenterOfMassInWorld_Z;
	else if (type == "LinearVelocity_X") return LinearVelocity_X;
	else if (type == "LinearVelocity_Y") return LinearVelocity_Y;
	else if (type == "LinearVelocity_Z") return LinearVelocity_Z;
	else if (type == "ResultantLinearVelocity") return ResultantLinearVelocity;
	else if (type == "KineticEnergy") return KineticEnergy;
	else if (type == "LinearImpulse_X") return LinearImpulse_X;
	else if (type == "LinearImpulse_Y") return LinearImpulse_Y;
	else if (type == "LinearImpulse_Z") return LinearImpulse_Z;
	else if (type == "ResultantLinearImpulse") return ResultantLinearImpulse;
	else if (type == "AngularVelocity_X") return AngularVelocity_X;
	else if (type == "AngularVelocity_Y") return AngularVelocity_Y;
	else if (type == "AngularVelocity_Z") return AngularVelocity_Z;
	else if (type == "ResultantAngularVelocity") return ResultantAngularVelocity;
	else if (type == "GravPotentialEnergy") return GravPotentialEnergy;
	else if (type == "TotalEnergy") return TotalEnergy;

	else return empty;
}

void DataKeeper::clearData( void )
{
	Time.clear();
	CenterOfMassInWorld_X.clear();
	CenterOfMassInWorld_Y.clear();
	CenterOfMassInWorld_Z.clear();
	LinearVelocity_X.clear();
	LinearVelocity_Y.clear();
	LinearVelocity_Z.clear();
	ResultantLinearVelocity.clear();
	KineticEnergy.clear();
	LinearImpulse_X.clear();
	LinearImpulse_Y.clear();
	LinearImpulse_Z.clear();
	ResultantLinearImpulse.clear();
	AngularVelocity_X.clear();
	AngularVelocity_Y.clear();
	AngularVelocity_Z.clear();
	ResultantAngularVelocity.clear();
	GravPotentialEnergy.clear();
	TotalEnergy.clear();
}

