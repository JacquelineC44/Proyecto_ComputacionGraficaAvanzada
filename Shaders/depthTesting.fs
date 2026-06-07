#version 330 core
out vec4 FragColor;

uniform float near = 0.1;
uniform float far = 100;
float LinearDepth(float depth){
    float z = depth * 2.0f -1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{        
    gl_FragCoord.z     
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);	
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
    // float depth = LinearDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);
    // gl_FragCoord.z
}
