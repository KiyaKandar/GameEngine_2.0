#include "Matrix4.h"

NCLMatrix4::NCLMatrix4(void)	{
	toIdentity();
}

NCLMatrix4::NCLMatrix4( float elements[16] )	{
	memcpy(this->values,elements,16*sizeof(float));
}

NCLMatrix4::~NCLMatrix4(void)	{
	toIdentity();
}

void NCLMatrix4::toIdentity() {
	toZero();
	values[0]  = 1.0f;
	values[5]  = 1.0f;
	values[10] = 1.0f;
	values[15] = 1.0f;
}

void NCLMatrix4::toZero()	{
	for(int i = 0; i < 16; i++)	{
		values[i] = 0.0f;
	}
}

NCLVector3 NCLMatrix4::getPositionVector() const{
	return NCLVector3(values[12],values[13],values[14]);
}

void	NCLMatrix4::setPositionVector(const NCLVector3 in) {
	values[12] = in.x;
	values[13] = in.y;
	values[14] = in.z;		
}

NCLVector3 NCLMatrix4::getScalingVector() const{
	return NCLVector3(values[0],values[5],values[10]);
}

void	NCLMatrix4::setScalingVector(const NCLVector3 &in) {
	values[0]  = in.x;
	values[5]  = in.y;
	values[10] = in.z;		
}

NCLMatrix4 NCLMatrix4::perspective(float znear, float zfar, float aspect, float fov) {
	NCLMatrix4 m;

	const float h = 1.0f / tan(fov*PI_OVER_360);
	float neg_depth = znear-zfar;

	m.values[0]		= h / aspect;
	m.values[5]		= h;
	m.values[10]	= (zfar + znear)/neg_depth;
	m.values[11]	= -1.0f;
	m.values[14]	= 2.0f*(znear*zfar)/neg_depth;
	m.values[15]	= 0.0f;

	return m;
}

//http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
NCLMatrix4 NCLMatrix4::orthographic(float znear, float zfar,float right, float left, float top, float bottom)	{
	NCLMatrix4 m;

	m.values[0]	= 2.0f / (right-left);
	m.values[5]	= 2.0f / (top-bottom);
	m.values[10]	= -2.0f / (zfar-znear);

	m.values[12]  = -(right+left)/(right-left);
	m.values[13]  = -(top+bottom)/(top-bottom);
	m.values[14]  = -(zfar+znear)/(zfar-znear);
	m.values[15]  = 1.0f;

	return m;
}

NCLMatrix4 NCLMatrix4::buildViewMatrix(const NCLVector3 &from, const NCLVector3 &lookingAt, const NCLVector3 up /*= Vector3(1,0,0)*/ )	{
	NCLMatrix4 r;
	r.setPositionVector(NCLVector3(-from.x,-from.y,-from.z));

	NCLMatrix4 m;

	NCLVector3 f = (lookingAt - from);
	f.normalise();

	NCLVector3 s = NCLVector3::cross(f,up);
	NCLVector3 u = NCLVector3::cross(s,f);

	s.normalise();
	u.normalise();

	m.values[0] = s.x;
	m.values[4] = s.y;
	m.values[8] = s.z;

	m.values[1] = u.x;
	m.values[5] = u.y;
	m.values[9] = u.z;

	m.values[2]  = -f.x;
	m.values[6]  = -f.y;
	m.values[10] = -f.z;

	return m*r;
}

NCLMatrix4 NCLMatrix4::rotation(float degrees, const NCLVector3 &inaxis)	 {
	NCLMatrix4 m;

	NCLVector3 axis = inaxis;

	axis.normalise();

	float c = cos((float)DegToRad(degrees));
	float s = sin((float)DegToRad(degrees));

	m.values[0]  = (axis.x * axis.x) * (1.0f - c) + c;
	m.values[1]  = (axis.y * axis.x) * (1.0f - c) + (axis.z * s);
	m.values[2]  = (axis.z * axis.x) * (1.0f - c) - (axis.y * s);

	m.values[4]  = (axis.x * axis.y) * (1.0f - c) - (axis.z * s);
	m.values[5]  = (axis.y * axis.y) * (1.0f - c) + c;
	m.values[6]  = (axis.z * axis.y) * (1.0f - c) + (axis.x * s);

	m.values[8]  = (axis.x * axis.z) * (1.0f - c) + (axis.y * s);
	m.values[9]  = (axis.y * axis.z) * (1.0f - c) - (axis.x * s);
	m.values[10] = (axis.z * axis.z) * (1.0f - c) + c;

	return m;
}

NCLMatrix4 NCLMatrix4::scale( const NCLVector3 &scale )	{
	NCLMatrix4 m;

	m.values[0]  = scale.x;
	m.values[5]  = scale.y;
	m.values[10] = scale.z;	

	return m;
}

NCLMatrix4 NCLMatrix4::translation( const NCLVector3 &translation )	{
	NCLMatrix4 m;

	m.values[12] = translation.x;
	m.values[13] = translation.y;
	m.values[14] = translation.z;	

	return m;
}

NCLMatrix4 NCLMatrix4::getTransposedRotation() {
	NCLMatrix4 temp;
	temp.values[0] = values[0];
	temp.values[5] = values[5];
	temp.values[10] = values[10];
	temp.values[1] = values[4];
	temp.values[4] = values[1];
	temp.values[2] = values[8];
	temp.values[8] = values[2];
	temp.values[6] = values[9];
	temp.values[9] = values[6];
	return temp;
}