#!mesh shader
#version 450
#extension GL_NV_mesh_shader : require

layout(local_size_x=1) in;
layout(max_vertices=8, max_primitives=12) out;
layout(triangles) out;

out gl_MeshPerVertexNV {
    vec4 gl_Position;
} gl_MeshVerticesNV[];

out uint gl_PrimitiveCountNV;
out uint gl_PrimitiveIndicesNV[];

// Se não redeclararmos gl_PerVertex, irá ocorrer o seguinte erro de compilação:
// error C7592: ARB_separate_shader_objects requires built-in block gl_PerVertex to be redeclared before accessing its members
out gl_PerVertex {
    vec4 gl_Position;
} gl_Why;

out PerVertexData
{
  vec4 color;
} v_out[];	

uniform mat4 MVP;

const vec3 vertices[8] = { 
    vec3(-1.0,-1.0,1.0), vec3(1.0,-1.0,1.0), vec3(-1.0,1.0,1.0), vec3(1.0,1.0,1.0),     //FRONT VERTICES
    vec3(-1.0,-1.0,-1.0), vec3(1.0,-1.0,-1.0), vec3(-1.0,1.0,-1.0), vec3(1.0,1.0,-1.0), //BACK VERTICES
};

const vec3 colors[4] = { 
    vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0), vec3(1.0,1.0,0.0) 
};

void main()
{
    gl_MeshVerticesNV[0].gl_Position = MVP * vec4(vertices[0], 1.0); // FRONT Upper Left
    gl_MeshVerticesNV[1].gl_Position = MVP * vec4(vertices[1], 1.0); // FRONT Upper Right
    gl_MeshVerticesNV[2].gl_Position = MVP * vec4(vertices[2], 1.0); // FRONT Bottom Left
    gl_MeshVerticesNV[3].gl_Position = MVP * vec4(vertices[3], 1.0); // FRONT Bottom Right

    gl_MeshVerticesNV[4].gl_Position = MVP * vec4(vertices[4], 1.0); // BACK Upper Left
    gl_MeshVerticesNV[5].gl_Position = MVP * vec4(vertices[5], 1.0); // BACK Upper Right
    gl_MeshVerticesNV[6].gl_Position = MVP * vec4(vertices[6], 1.0); // BACK Bottom Left
    gl_MeshVerticesNV[7].gl_Position = MVP * vec4(vertices[7], 1.0); // BACK Bottom Right

	// Vertice Colors
	v_out[0].color = vec4(colors[0], 1.0);
  	v_out[1].color = vec4(colors[1], 1.0);
  	v_out[2].color = vec4(colors[2], 1.0);
  	v_out[3].color = vec4(colors[3], 1.0);
    v_out[4].color = vec4(colors[3], 1.0);
  	v_out[5].color = vec4(colors[2], 1.0);
  	v_out[6].color = vec4(colors[1], 1.0);
  	v_out[7].color = vec4(colors[0], 1.0);

	// FRONT FACE First Triangle
    gl_PrimitiveIndicesNV[0] = 0;
    gl_PrimitiveIndicesNV[1] = 1;
    gl_PrimitiveIndicesNV[2] = 2;
	gl_PrimitiveCountNV++;

	// FRONT FACE Second Triangle
    gl_PrimitiveIndicesNV[3] = 2;
    gl_PrimitiveIndicesNV[4] = 1;
    gl_PrimitiveIndicesNV[5] = 3;
    gl_PrimitiveCountNV++;

    // BACK FACE First Triangle
    gl_PrimitiveIndicesNV[6] = 4;
    gl_PrimitiveIndicesNV[7] = 5;
    gl_PrimitiveIndicesNV[8] = 6;
	gl_PrimitiveCountNV++;

	// BACK FACE Second Triangle
    gl_PrimitiveIndicesNV[9]  = 6;
    gl_PrimitiveIndicesNV[10] = 5;
    gl_PrimitiveIndicesNV[11] = 7;
    gl_PrimitiveCountNV++;

    // TOP FACE First Triangle
    gl_PrimitiveIndicesNV[12] = 0;
    gl_PrimitiveIndicesNV[13] = 1;
    gl_PrimitiveIndicesNV[14] = 4;
	gl_PrimitiveCountNV++;

	// TOP FACE Second Triangle
    gl_PrimitiveIndicesNV[15] = 4;
    gl_PrimitiveIndicesNV[16] = 5;
    gl_PrimitiveIndicesNV[17] = 0;
    gl_PrimitiveCountNV++;

    // BOTTOM FACE First Triangle
    gl_PrimitiveIndicesNV[18] = 12;
    gl_PrimitiveIndicesNV[19] = 14;
    gl_PrimitiveIndicesNV[20] = 13;
	gl_PrimitiveCountNV++;

	// BOTTOM FACE Second Triangle
    gl_PrimitiveIndicesNV[21] = 13;
    gl_PrimitiveIndicesNV[22] = 14;
    gl_PrimitiveIndicesNV[23] = 15;
    gl_PrimitiveCountNV++;

    // LEFT FACE First Triangle
    gl_PrimitiveIndicesNV[24] = 16;
    gl_PrimitiveIndicesNV[25] = 18;
    gl_PrimitiveIndicesNV[26] = 17;
	gl_PrimitiveCountNV++;

	// LEFT FACE Second Triangle
    gl_PrimitiveIndicesNV[27] = 17;
    gl_PrimitiveIndicesNV[28] = 18;
    gl_PrimitiveIndicesNV[29] = 19;
    gl_PrimitiveCountNV++;

    // RIGTH FACE First Triangle
    gl_PrimitiveIndicesNV[30] = 20;
    gl_PrimitiveIndicesNV[31] = 21;
    gl_PrimitiveIndicesNV[32] = 22;
	gl_PrimitiveCountNV++;

	// RIGTH FACE Second Triangle
    gl_PrimitiveIndicesNV[33] = 21;
    gl_PrimitiveIndicesNV[34] = 23;
    gl_PrimitiveIndicesNV[35] = 22;
    gl_PrimitiveCountNV++;
}
  

#!fragment shader
#version 450
 
layout(location = 0) out vec4 FragColor;
 
in PerVertexData
{
	vec4 color;
} fragIn;	
 
void main()
{
	FragColor = fragIn.color;
}