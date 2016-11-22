#version 150

uniform sampler2D texUnit1;
uniform sampler2D texUnit2;

in vec2 outTexCoord;
out vec4 fragColor;

uniform float width; // the width of a patch
uniform float height; // the height of a patch
uniform float offset_w; // the size of the overlap width
uniform float offset_h; // the size of the overlap height

uniform int side; // 0 = left side. 1 = down side

//Convert RGB to HSV color space
//Formula taken form: http://www.easyrgb.com/index.php?X=MATH&H=20#text20 (November 2016)
vec4 RGB2HSV(vec4 v)
{
	float varMin = min( min(v.x, v.y), v.z );	//Min. value of RGB
	float varMax = max( max(v.x, v.y), v.z );	//Max. value of RGB
	float delMax = varMax - varMin;				//Delta RGB value

	float H = 0.0;	//HSV results from 0 to 1
	float S = 0.0;
	float V = varMax;

	if ( delMax != 0 )	//We are only interested in chromatic data (not grayscale)
	{
		S = delMax / varMax;

		float delR = ( ( ( varMax - v.x ) / 6 ) + ( delMax / 2 ) ) / delMax;
		float delG = ( ( ( varMax - v.y ) / 6 ) + ( delMax / 2 ) ) / delMax;
		float delB = ( ( ( varMax - v.z ) / 6 ) + ( delMax / 2 ) ) / delMax;

		if		( v.x == varMax ) H = delB - delG;
		else if	( v.y == varMax ) H = ( 1 / 3 ) + delR - delB;
		else if	( v.z == varMax ) H = ( 2 / 3 ) + delG - delR;

		if ( H < 0 ) H += 1;
		if ( H > 1 ) H -= 1;
	}

	return vec4(H, S, V, 0.0);
}

//Convert RGB to HSL color space
//Formula taken form: http://www.easyrgb.com/index.php?X=MATH&H=18#text18 (November 2016)
vec4 RGB2HSL(vec4 v)
{
	float varMin = min( min(v.x, v.y), v.z );	//Min. value of RGB
	float varMax = max( max(v.x, v.y), v.z );	//Max. value of RGB
	float delMax = varMax - varMin;				//Delta RGB value

	float H = 0.0;	//HSV results from 0 to 1
	float S = 0.0;
	float L = (varMax + varMin) / 2;

	if ( delMax != 0 )	//We are only interested in chromatic data (not grayscale)
	{
		if ( L < 0.5 )	S = delMax / ( varMax + varMin );
   		else			S = delMax / ( 2 - varMax - varMin );

		float delR = ( ( ( varMax - v.x ) / 6 ) + ( delMax / 2 ) ) / delMax;
		float delG = ( ( ( varMax - v.y ) / 6 ) + ( delMax / 2 ) ) / delMax;
		float delB = ( ( ( varMax - v.z ) / 6 ) + ( delMax / 2 ) ) / delMax;

		if		( v.x == varMax ) H = delB - delG;
		else if	( v.y == varMax ) H = ( 1 / 3 ) + delR - delB;
		else if	( v.z == varMax ) H = ( 2 / 3 ) + delG - delR;

		if ( H < 0 ) H += 1;
		if ( H > 1 ) H -= 1;
	}

	return vec4(H, S, L, 0.0);
}

//Convert RGB to Lab color space
// Taken form: https://github.com/sv3/gift/blob/master/RGB2Lab.glsl (November 2016)
vec4 RGB2Lab(in vec4 rgb){
    float R = rgb.x;
    float G = rgb.y;
    float B = rgb.z;
    // threshold
    float T = 0.008856;

    float X = R * 0.412453 + G * 0.357580 + B * 0.180423;
    float Y = R * 0.212671 + G * 0.715160 + B * 0.072169;
    float Z = R * 0.019334 + G * 0.119193 + B * 0.950227;

    // Normalize for D65 white point
    X = X / 0.950456;
    Y = Y;
    Z = Z / 1.088754;

    bool XT, YT, ZT;
    XT = false; YT=false; ZT=false;
    if(X > T) XT = true;
    if(Y > T) YT = true;
    if(Z > T) ZT = true;

    float Y3 = pow(Y,1.0/3.0);
    float fX, fY, fZ;
    if(XT){ fX = pow(X, 1.0/3.0);} else{ fX = 7.787 * X + 16.0/116.0; }
    if(YT){ fY = Y3; } else{ fY = 7.787 * Y + 16.0/116.0 ; }
    if(ZT){ fZ = pow(Z,1.0/3.0); } else{ fZ = 7.787 * Z + 16.0/116.0; }

    float L; if(YT){ L = (116.0 * Y3) - 16.0; }else { L = 903.3 * Y; }
    float a = 500.0 * ( fX - fY );
    float b = 200.0 * ( fY - fZ );

    return vec4(L, a, b, 0.0);
}

float diffFunction(vec4 a, vec4 b)
{
	//a = RGB2HSV(a);
	//b = RGB2HSV(b);
	//a = RGB2HSL(a);
	//b = RGB2HSL(b);
	a = RGB2Lab(a);
	b = RGB2Lab(b);

	float res = pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2);
	return res;
}


void main(void)
{
	fragColor = vec4(0.0);	
	if(side == 0 && outTexCoord.x < offset_w) {
		vec2 texCoord1 = outTexCoord;
		vec2 texCoord2 = outTexCoord + vec2(width - offset_w, 0.0);
		
		vec4 a  = texture(texUnit1, texCoord1); // left patch
		vec4 b  = texture(texUnit2, texCoord2); // right patch
		fragColor.x = diffFunction(a, b);
		//fragColor = vec4(pow(a.x - b.x, 2), pow(a.y - b.y, 2), pow(a.z - b.z, 2), 1.0);
	}
	else if(side == 1 && outTexCoord.y < offset_h) {
		vec2 texCoord1 = outTexCoord + vec2(0.0, height - offset_h);
		vec2 texCoord2 = outTexCoord;
		
		vec4 a  = texture(texUnit1, texCoord1); // left patch
		vec4 b  = texture(texUnit2, texCoord2); // right patch

		fragColor.x = diffFunction(a, b);
		//fragColor = rgbToHSV(vec4(pow(a.x - b.x, 2), pow(a.y - b.y, 2), pow(a.z - b.z, 2), 0.0));
	}
	else {
		fragColor = vec4(0.0);	
	}
}

