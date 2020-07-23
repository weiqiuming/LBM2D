#ifndef LBM_H
#define LBM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class LBM
{
    public:
		struct lbm_node
		{
			float rho;
			float f[9];
			vec2 u;
			uint type;
		};
		struct d3q9_node
		{
			vec2 e;
			float w;
			uint op;
			void set(vec2 a,float b,uint c)
			{
				e = a;
				w = b;
				op = c;
			}
		};
	
		vec3* vertexData;
		uint* indexData;
        uint width;
        uint height;
		lbm_node* LBMData;
		lbm_node* LBMTempData;
		d3q9_node* d3q9;

        LBM(uint w,uint h);
		
        ~LBM();
        void simulation();

		vec3* getVertexData();
		uint* getIndexData();
    private:

};

#endif  
