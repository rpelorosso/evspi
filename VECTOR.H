
#ifndef _vector_
#define _vector_

typedef struct
{
	 float x,y,z;
} vector3;

typedef struct
{
	 float x,y;
} vector2;

vector3 cvector3(float x, float y, float z);
vector2 cvector2(float x, float y);
vector3 addvector3(vector3 a, vector3 b);
//inline vector3 subvector3(vector3 a, vector3 b);
vector3 equvector(vector3 a);
float dotproduct(vector3 a, vector3 b);

vector3 cvector3(float x, float y, float z)
{
	 vector3 v;
	 v.x=x; v.y=y; v.z=z;
	 return v;
};

vector2 cvector2(float x, float y)
{
	 vector2 v;
	 v.x=x; v.y=y;
	 return v;
}

vector3 addvector3(vector3 a, vector3 b)
{
	 vector3 v;
	 v.x=a.x+b.x;v.y=a.y+b.y;v.z=a.z+b.z;
	 return v;
}

vector3 subvector3(vector3 a, vector3 b)
{
	 vector3 v;
	 v.x=a.x-b.x;v.y=a.y-b.y;v.z=a.z-b.z;
	 return v;
}

float dotproduct(vector3 a, vector3 b)
{
 return (a.x*b.x+a.y*b.y+a.z*b.z);
}

vector3 equvector(vector3 a)
{
 return a;
}

vector3 normalize(vector3 v)
{
vector3 r;
double len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
r.x /= len;
r.y /= len;
r.z /= len;
return r;
}



#endif
