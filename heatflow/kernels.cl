void loadInitial(float sum[3], float center[3], int width, int y, __global const float* restrict in)
{
	const int x = 0;
	sum[0] =  in[(y-1)*width+x-1];
	sum[0] += in[(y-1)*width+x+0];
	sum[0] += in[(y-1)*width+x+1];

	sum[1] =  in[y*width+x-1];
	sum[1] += in[y*width+x+0];
	sum[1] += in[y*width+x+1];

	sum[2] =  in[(y+1)*width+x-1];
	sum[2] += in[(y+1)*width+x+0];
	sum[2] += in[(y+1)*width+x+1];

	center[1] = in[y*width+x+0];
	center[2] = in[(y+1)*width+x+0];
}


void load(float sum[3], float center[3], int width, int y, int x, __global const float* restrict in)
{
	int mod = x%3;

	sum[mod] = in[(y-1)*width+x+0];
	sum[mod] += in[(y+0)*width+x];
	sum[mod] += in[(y+1)*width+x];

	center[mod] = in[(y+0)*width+x];
}

float calc(float sum[3], float center[3], int x)
{
	int mod = x%3;
	return sum[0] + sum[1] + sum[2] - center[mod];
}

__kernel void heatstep(__global float* restrict in, __global float* restrict out
	, int width, int height)
{
	float K = 0.5f;
	int y = get_global_id(0);
	if( y == 0 || y == height-1)
		return;
	float tmp[3];
	float center[3];
	loadInitial(tmp, center, width, y, in);
	for(int x=1;x<width-1;x++)
	{
		int mod = x%3;
		float surroundings = calc(tmp, center, x)*0.125f;
		out[y*width+x] = center[mod] - (K * (center[mod] - surroundings));

		if(x < width-2)
			load(tmp, center, width, y, x+2, in);
	}
}

