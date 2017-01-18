// begin fastpow2
float log2(float i)
	{
		const float log2=0.6931471805599453094172321214581765680755001343602552;//ln(2)
	return std::logf(i)/log2;
	}
float shift23=(1<<23);
float OOshift23=1.0/(1<<23);

float myLog2(float i)
	{
	float LogBodge=0.346607f;
	float x;
	float y;
	x=*(int *)&i;
	x*= OOshift23; //1/pow(2,23);
	x=x-127;

	y=x-floorf(x);
	y=(y-y*y)*LogBodge;
	return x+y;
	}
float myPow2(float i)
	{
	float PowBodge=0.33971f;
	float x;
	float y=i-floorf(i);
	y=(y-y*y)*PowBodge;

	x=i+127-y;
	x*= shift23; //pow(2,23);
	*(int*)&x=(int)x;
	return x;
	}

float myPow(float a, float b)
	{
	return myPow2(b*myLog2(a));
	}
//end fastpow2