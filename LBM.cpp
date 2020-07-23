#include "include/LBM.h"
#include <iostream>
#include <stdlib.h>

#define BOUNDARY 1
#define SOLID 1<<1
#define SOURCE 1<<2
#define FLUID 1<<3

#define OMEGA 1.8f
using namespace std;

void stream(LBM::lbm_node* input,LBM::lbm_node* output,LBM::d3q9_node* d3q9,uint width,uint height);
void collide(LBM::lbm_node* input,LBM::lbm_node* output,LBM::d3q9_node* d3q9,uint width,uint height);
void init(LBM::lbm_node* data1,LBM::lbm_node* data2,LBM::d3q9_node* d3q9,uint width,uint height);


LBM::LBM(uint w,uint h)
{
	width = w;
	height = h;

	//malloc memory
	vertexData = (vec3*)malloc(sizeof(vec3)*w*h);
	indexData = (uint*)malloc(sizeof(uint)*(w-1)*(h-1)*6);
	LBMData = (LBM::lbm_node*)malloc(sizeof(LBM::lbm_node)*w*h);
	LBMTempData = (LBM::lbm_node*)malloc(sizeof(LBM::lbm_node)*w*h);
	d3q9 = (LBM::d3q9_node*)malloc(sizeof(LBM::d3q9_node)*9);

	//init d3q9
	d3q9[0].set(vec2(0,0),4.0f/9.0f,0); 
	d3q9[1].set(vec2(1,0),1.0f/9.0f,4); 
	d3q9[2].set(vec2(0,1),1.0f/9.0f,3); 
	d3q9[3].set(vec2(0,-1),1.0f/9.0f,2); 
	d3q9[4].set(vec2(-1,0),1.0f/9.0f,1); 
	d3q9[5].set(vec2(1,1),1.0f/36.0f,7); 
	d3q9[6].set(vec2(-1,1),1.0f/36.0f,8); 
	d3q9[7].set(vec2(-1,-1),1.0f/36.0f,5); 
	d3q9[8].set(vec2(1,-1),1.0f/36.0f,6);

	init(LBMData,LBMTempData,d3q9,width,height);
}

LBM::~LBM()
{
	free(vertexData);
	free(indexData);
	free(LBMData);
	free(LBMTempData);
	free(d3q9);

}

vec3* LBM::getVertexData()
{
	int i=0,j=0;
	uint offset = 0;
	while(j<height)
	{
		while(i<width)
		{
			offset = i + j * width;
			if(LBMData[offset].type&(FLUID|SOURCE))
			{
				vertexData[offset] = vec3(i,LBMData[offset].rho-1.0f,j);
			}
			else
			{
				vertexData[offset] = vec3(i,0,j);
			}
			i++;
		}
		i=0;
		j++;
	}
	return vertexData;
}

void LBM::simulation()
{
	stream(this->LBMData,this->LBMTempData,this->d3q9,this->width,this->height);
	collide(this->LBMTempData,this->LBMData,this->d3q9,this->width,this->height);
}

float calFeq(vec2 u,vec2 e)
{
	float u2 = u.x * u.x + u.y*u.y;
	float ue = u.x * e.x + u.y*e.y;
	return 1.0f + 3*ue + 4.5f*ue*ue -1.5f*u2;
}

void init(LBM::lbm_node* data1,LBM::lbm_node* data2,LBM::d3q9_node* d3q9,uint width,uint height)
{
	uint i=0,j=0,offset=0;
	while(j<height)
	{
		while(i<width)
		{
			uint offset = i + j *width;
			bool isBoundary = (i==0)||(j==0)||(i==width-1)||(j==height-1);
			bool isSource = ((i-width/2)*(i-width/2)+(j-height/2)*(j-height/2))<30;
			bool isSolid = false;
			if(isBoundary||isSolid)
			{	
				data1[offset].rho = 0.0f;
				data1[offset].u = vec2(0,0);
				data1[offset].type = BOUNDARY;
				uint dir=0;
				while(dir<9)
				{
					data1[offset].f[dir] = 0;
					data2[offset].f[dir] = 0;
					dir++;
				}

				data2[offset].rho = 0.0f;
				data2[offset].u = vec2(0,0);
				data2[offset].type = BOUNDARY;
			}
			else if(isSource)
			{
				data1[offset].rho = 2.0f;
				data1[offset].u = vec2(0,0);
				data1[offset].type = SOURCE;
				
				data2[offset].rho = 2.0f;
				data2[offset].u = vec2(0,0);
				data2[offset].type = SOURCE;

				uint dir =0;
				while(dir<9)
				{
					data1[offset].f[dir] = data1[offset].rho * d3q9[dir].w*calFeq(data1[offset].u,d3q9[dir].e);
					data2[offset].f[dir] = data2[offset].rho * d3q9[dir].w*calFeq(data2[offset].u,d3q9[dir].e);
					dir++;
				}
			}
			else
			{
				data1[offset].rho = 1.0f;
				data1[offset].u = vec2(0,0);
				data1[offset].type = FLUID;
				data2[offset].rho = 1.0f;
				data2[offset].u = vec2(0,0);
				data2[offset].type = FLUID;

				uint dir =0;
				while(dir<9)
				{
					data1[offset].f[dir] = data1[offset].rho * d3q9[dir].w*calFeq(data1[offset].u,d3q9[dir].e);
					data2[offset].f[dir] = data2[offset].rho * d3q9[dir].w*calFeq(data2[offset].u,d3q9[dir].e);
					dir++;
				}
				
					}
			i++;
		}
		i=0;
		j++;
	}
}
void stream(LBM::lbm_node* input,LBM::lbm_node* output,LBM::d3q9_node* d3q9,uint width,uint height)
{
	uint i=0,j=0,offset=0;
	while(j<height)
	{
		while(i<width)
		{
			uint offset = i + j *width;
			if(input[offset].type&FLUID)
			{		
				float rho =0.0f;
				vec2 u = vec2(0,0);
				
				uint dir=0;
				while(dir<9)
				{
					uint tempOffset = i + d3q9[dir].e.x + (j+d3q9[dir].e.y)*width;
					if(input[tempOffset].type&(SOLID|BOUNDARY))
					{
						output[offset].f[d3q9[dir].op]= input[offset].f[dir];
						output[offset].f[d3q9[dir].op]= input[offset].f[dir];

					}
					else if(input[tempOffset].type&SOURCE)
					{
						output[offset].f[d3q9[dir].op]= input[tempOffset].f[d3q9[dir].op];
					}
					else if(input[tempOffset].type&FLUID)
					{
						output[offset].f[d3q9[dir].op]= input[tempOffset].f[d3q9[dir].op];
					}
					rho += output[offset].f[d3q9[dir].op];
					u.x += output[offset].f[d3q9[dir].op]*d3q9[d3q9[dir].op].e.x;
					u.y += output[offset].f[d3q9[dir].op]*d3q9[d3q9[dir].op].e.y;

					dir++;
				}

				output[offset].rho = rho;
				if(rho>0)
				{
					u.x = u.x /rho;
					u.y = u.y /rho;
				}
				else
				{
					u = vec2(0,0);
				}
				output[offset].u =u;
			}
			else
			{

			}
			i++;
		}
		i=0;
		j++;
	}
}

void collide(LBM::lbm_node* input,LBM::lbm_node* output,LBM::d3q9_node* d3q9,uint width,uint height)
{
	uint i=0,j=0,offset=0;
	while(j<height)
	{
		while(i<width)
		{
			uint offset = i + j *width;
			if(input[offset].type&FLUID)
			{
				output[offset].rho = input[offset].rho;
				output[offset].u = input[offset].u;
				
				uint dir =0;
				while(dir<9)
				{
					output[offset].f[dir] = input[offset].f[dir] - 
						OMEGA*(input[offset].f[dir]-output[offset].rho*d3q9[dir].w*calFeq(output[offset].u,d3q9[dir].e));
					dir++;
				}
			}
			else if(input[offset].type&SOURCE)
			{
				output[offset].rho = 2.0f;
				output[offset].u = vec2(0,0);

				uint dir =0;
				while(dir<9)
				{
					output[offset].f[dir] = output[offset].rho*d3q9[dir].w*calFeq(output[offset].u,d3q9[dir].e);
					dir++;
				}
			
			}
			else
			{
				output[offset].rho =0.0f;
			}
			i++;
		}
		i=0;
		j++;
	}
}
uint* LBM::getIndexData()
{
	int i =1,j=1;
	while(j<height)
	{
		while(i<width)
		{
			uint index = i - 1 + (j-1) * width;
			indexData[index*6] = i+j*width;
			indexData[index*6+1] = i-1 + j*width;
			indexData[index*6+2] = i-1 + (j-1)*width;
			indexData[index*6+3] = i+j*width;
			indexData[index*6+4] = i-1 + (j-1)*width;
			indexData[index*6+5] = i+(j-1)*width;
			i++;
		}
		i=1;
		j++;
	}
	return indexData;
}
