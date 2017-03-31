#version 410

in vec4 Normal;
in vec3 vertexPos;

out vec4 outColor;

uniform vec4 color = vec4(1);
uniform vec3 light = vec3(0,-10,100);
uniform vec3 cameraPos;

void main()
{
  outColor = color;

		/*outColor = vec4(1);
		//if(length(Normal==1)
			vec3 l = vec3(light-vPos);
			l = normalize(l);
			vec3 c = vec3(color);
			vec3 n = vec3(Normal);
			vec3 e = cameraPos-vPos;
			e = -normalize(e);
			vec3 h = normalize(e+l);
			if(dot(n,l)<0)
				n=-n;
			if(test)
				outColor = vec4(vec3(0.01)*max(0,pow(dot(h,n),1)),1.f);
			else
    		outColor = vec4(c*(vec3(0.2)+max(0,dot(n,l))) +vec3(0.01)*max(0,pow(dot(h,n), 1)), 1);
			*/
}
