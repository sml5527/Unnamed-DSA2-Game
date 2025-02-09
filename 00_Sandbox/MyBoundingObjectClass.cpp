#include "MyBoundingObjectClass.h"
//  MyBoundingObjectClass
void MyBoundingObjectClass::Init(void)
{
	m_fRadius = 0.0f;
	m_m4ToWorld = IDENTITY_M4;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_m4ToWorld = IDENTITY_M4;

	m_v3CenterG = vector3(0.0f);
	m_v3MinG = vector3(0.0f);
	m_v3MaxG = vector3(0.0f);

	m_v3HalfWidth = vector3(0.0f);
	m_v3HalfWidthG = vector3(0.0f);

	m_fRadius = 0.0f;

	m_pMeshMngr = MeshManagerSingleton::GetInstance();
}
void MyBoundingObjectClass::Swap(MyBoundingObjectClass& other)
{
	std::swap(m_fRadius, other.m_fRadius);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);
}
void MyBoundingObjectClass::Release(void)
{

}
//The big 3
MyBoundingObjectClass::MyBoundingObjectClass(std::vector<vector3> a_lVectorList)
{
	//Init the default values
	Init();

	//Count the points of the incoming list
	uint nVertexCount = a_lVectorList.size();

	//If there are none just return, we have no information to create the BS from
	if (nVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3Max = m_v3Min = a_lVectorList[0];

	//Get the max and min out of the list
	for (uint nVertex = 1; nVertex < nVertexCount; nVertex++)
	{
		if (m_v3Min.x > a_lVectorList[nVertex].x) //If min is larger than current
			m_v3Min.x = a_lVectorList[nVertex].x;
		else if (m_v3Max.x < a_lVectorList[nVertex].x)//if max is smaller than current
			m_v3Max.x = a_lVectorList[nVertex].x;

		if (m_v3Min.y > a_lVectorList[nVertex].y) //If min is larger than current
			m_v3Min.y = a_lVectorList[nVertex].y;
		else if (m_v3Max.y < a_lVectorList[nVertex].y)//if max is smaller than current
			m_v3Max.y = a_lVectorList[nVertex].y;

		if (m_v3Min.z > a_lVectorList[nVertex].z) //If min is larger than current
			m_v3Min.z = a_lVectorList[nVertex].z;
		else if (m_v3Max.z < a_lVectorList[nVertex].z)//if max is smaller than current
			m_v3Max.z = a_lVectorList[nVertex].z;
	}

	//with the max and the min we calculate the center
	m_v3Center = (m_v3Min + m_v3Max) / 2.0f;

	m_fRadius = glm::distance(m_v3Center, m_v3Max);

	//we calculate the distance between all the values of min and max vectors
	m_v3HalfWidth = (m_v3Max - m_v3Min) / 2.0f;

	m_v3MaxG = m_v3Max;
	m_v3MinG = m_v3Min;
	m_v3CenterG = m_v3Center;
	m_v3HalfWidthG = m_v3HalfWidth;
}
MyBoundingObjectClass::MyBoundingObjectClass(MyBoundingObjectClass const& other)
{
	m_fRadius = other.m_fRadius;
	m_m4ToWorld = other.m_m4ToWorld;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;
}
MyBoundingObjectClass& MyBoundingObjectClass::operator=(MyBoundingObjectClass const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyBoundingObjectClass temp(other);
		Swap(temp);
	}
	return *this;
}
MyBoundingObjectClass::~MyBoundingObjectClass() { Release(); };
//Accessors
void MyBoundingObjectClass::SetModelMatrix(matrix4 a_m4ToWorld) { m_m4ToWorld = a_m4ToWorld; }
vector3 MyBoundingObjectClass::GetCenterG(void) { return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
float MyBoundingObjectClass::GetRadius(void) { return m_fRadius; }
vector3 MyBoundingObjectClass::GetSize(void) { return m_v3Size; };
vector3 MyBoundingObjectClass::GetHalfWidth(void) { return m_v3HalfWidth; }
vector3 MyBoundingObjectClass::GetHalfWidthG(void) { return m_v3HalfWidthG; }
//--- Non Standard Singleton Methods
bool MyBoundingObjectClass::IsColliding(MyBoundingObjectClass* const a_pOther)
{
	//Get all vectors in global space
	vector3 v3Min = vector3(m_m4ToWorld * vector4(m_v3Min, 1.0f));
	vector3 v3Max = vector3(m_m4ToWorld * vector4(m_v3Max, 1.0f));

	vector3 v3MinO = vector3(a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Min, 1.0f));
	vector3 v3MaxO = vector3(a_pOther->m_m4ToWorld * vector4(a_pOther->m_v3Max, 1.0f));

	/*
	Are they colliding?
	For Objects we will assume they are colliding, unless at least one of the following conditions is not met
	*/
	//first check the bounding sphere, if that is not colliding we can guarantee that there are no collision
	if ((m_fRadius + a_pOther->m_fRadius) < glm::distance(m_v3CenterG, a_pOther->m_v3CenterG))
		return false;

	//If the distance was smaller it might be colliding

	bool bColliding = true;


	//Check for X
	if (m_v3MaxG.x < a_pOther->m_v3MinG.x)
		bColliding = false;
	if (m_v3MinG.x > a_pOther->m_v3MaxG.x)
		bColliding = false;

	//Check for Y
	if (m_v3MaxG.y < a_pOther->m_v3MinG.y)
		bColliding = false;
	if (m_v3MinG.y > a_pOther->m_v3MaxG.y)
		bColliding = false;

	//Check for Z
	if (m_v3MaxG.z < a_pOther->m_v3MinG.z)
		bColliding = false;
	if (m_v3MinG.z > a_pOther->m_v3MaxG.z)
		bColliding = false;

	if (bColliding == true)
	{

		//starting OBB test
		float aTempRadius, bTempRadius;
		matrix3 rotate, absRotate;
		//compute rotation matrix expressing b in a's coord frame
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				rotate[i][j] = glm::dot(GetHalfWidth()[i], a_pOther->GetHalfWidth()[j]);

			}

		}

		//get translation vector t (called distanceV in this case)
		vector3 distanceV = a_pOther->GetCenterG() - GetCenterG();
		distanceV = vector3(glm::dot(distanceV[0], GetHalfWidth()[0]), glm::dot(distanceV[2], GetHalfWidth()[2]), glm::dot(distanceV[2], GetHalfWidth()[2])); // had to add array notation to distanceV here.

																																							  //compute common subexpressions
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				absRotate[i][j] = glm::abs(rotate[i][j]) + .00001;

			}

		}


		// Test axes L = A0, L = A1, L = A2 
		for (int i = 0; i < 3; i++) {
			aTempRadius = GetHalfWidthG()[i];
			bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[i][0] + a_pOther->GetHalfWidthG()[1] * absRotate[i][1] + a_pOther->GetHalfWidthG()[2] * absRotate[i][2];
			if (abs(distanceV[i]) > aTempRadius + bTempRadius) return false;
		}

		// Test axes L = B0, L = B1, L = B2 
		for (int i = 0; i < 3; i++) {
			aTempRadius = GetHalfWidthG()[0] * absRotate[0][i] + GetHalfWidthG()[1] * absRotate[1][i] + GetHalfWidthG()[2] * absRotate[2][i];
			bTempRadius = a_pOther->GetHalfWidthG()[i];
			if (abs(distanceV[0] * rotate[0][i] + distanceV[1] * rotate[1][i] + distanceV[2] * rotate[2][i]) > aTempRadius + bTempRadius) return false;
		}

		// Test axis L = A0 x B0 
		aTempRadius = GetHalfWidthG()[1] * absRotate[2][0] + GetHalfWidthG()[2] * absRotate[1][0];
		bTempRadius = a_pOther->GetHalfWidthG()[1] * absRotate[0][2] + a_pOther->GetHalfWidthG()[2] * absRotate[0][1];
		if (abs(distanceV[2] * rotate[1][0] - distanceV[1] * rotate[2][0]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A0 x B1 
		aTempRadius = GetHalfWidthG()[1] * absRotate[2][1] + GetHalfWidthG()[2] * absRotate[1][1];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[0][2] + a_pOther->GetHalfWidthG()[2] * absRotate[0][0];
		if (abs(distanceV[2] * rotate[1][1] - distanceV[1] * rotate[2][1]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A0 x B2 
		aTempRadius = GetHalfWidthG()[1] * absRotate[2][2] + GetHalfWidthG()[2] * absRotate[1][2];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[0][1] + a_pOther->GetHalfWidthG()[1] * absRotate[0][0];
		if (abs(distanceV[2] * rotate[1][2] - distanceV[1] * rotate[2][2]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A1 x B0 
		aTempRadius = GetHalfWidthG()[0] * absRotate[2][0] + GetHalfWidthG()[2] * absRotate[0][0];
		bTempRadius = a_pOther->GetHalfWidthG()[1] * absRotate[1][2] + a_pOther->GetHalfWidthG()[2] * absRotate[1][1];
		if (abs(distanceV[0] * rotate[2][0] - distanceV[2] * rotate[0][0]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A1 x B1 
		aTempRadius = GetHalfWidthG()[0] * absRotate[2][1] + GetHalfWidthG()[2] * absRotate[0][1];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[1][2] + a_pOther->GetHalfWidthG()[2] * absRotate[1][0];
		if (abs(distanceV[0] * rotate[2][1] - distanceV[2] * rotate[0][1]) > aTempRadius + bTempRadius) return false;


		// Test axis L = A1 x B2 
		aTempRadius = GetHalfWidthG()[0] * absRotate[2][2] + GetHalfWidthG()[2] * absRotate[0][2];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[1][1] + a_pOther->GetHalfWidthG()[1] * absRotate[1][0];
		if (glm::abs(distanceV[0] * rotate[2][2] - distanceV[2] * rotate[0][2]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A2 x B0 
		aTempRadius = GetHalfWidthG()[0] * absRotate[1][0] + GetHalfWidthG()[1] * absRotate[0][0];
		bTempRadius = a_pOther->GetHalfWidthG()[1] * absRotate[2][2] + a_pOther->GetHalfWidthG()[2] * absRotate[2][1];
		if (glm::abs(distanceV[1] * rotate[0][0] - distanceV[0] * rotate[1][0]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A2 x B1 
		aTempRadius = GetHalfWidthG()[0] * absRotate[1][1] + GetHalfWidthG()[1] * absRotate[0][1];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[2][2] + a_pOther->GetHalfWidthG()[2] * absRotate[2][0];
		if (glm::abs(distanceV[1] * rotate[0][1] - distanceV[0] * rotate[1][1]) > aTempRadius + bTempRadius) return false;

		// Test axis L = A2 x B2 
		aTempRadius = GetHalfWidthG()[0] * absRotate[1][2] + GetHalfWidthG()[1] * absRotate[0][2];
		bTempRadius = a_pOther->GetHalfWidthG()[0] * absRotate[2][1] + a_pOther->GetHalfWidthG()[1] * absRotate[2][0];
		if (glm::abs(distanceV[1] * rotate[0][2] - distanceV[0] * rotate[1][2]) > aTempRadius + bTempRadius) return false;


		//if no separating axis is found, they must be colliding
		std::cout << "hit";
		return true;
	}
	else
		return false;
}

void MyBoundingObjectClass::draw(void)
{
	if(visibility)
	{
		m_pMeshMngr->AddCubeToQueue(glm::translate(vector3(GetCenterG()))*glm::scale(vector3(GetSize())), color, WIRE);
		m_pMeshMngr->AddSphereToQueue(glm::translate(vector3(GetCenterG()))*glm::scale(vector3(GetRadius()* 2.0f)), color, WIRE);
	}
	

}
matrix4 MyBoundingObjectClass::GetModelMatrix(void) { return m_m4ToWorld; }
void MyBoundingObjectClass::setModelMatrix(matrix4 value) {m_m4ToWorld =value; }

void MyBoundingObjectClass::setVisibility(bool value)
{
	visibility = value;
}


void MyBoundingObjectClass::setColor(vector3 value)
{
	color = value;
}

