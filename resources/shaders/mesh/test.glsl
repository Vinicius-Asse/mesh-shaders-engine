#!mesh shader
#version 450
#extension GL_NV_mesh_shader : require

layout(local_size_x=1) in;
layout(max_vertices=4, max_primitives=2) out;
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

const vec3 colors[4] = { vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0), vec3(1.0, 1.0, 0.0) };

void main()
{
    gl_MeshVerticesNV[0].gl_Position = vec4(-1.0, -1.0, 0.0, 1.0); // Upper Left
    gl_MeshVerticesNV[1].gl_Position = vec4( 1.0, -1.0, 0.0, 1.0); // Upper Right
    gl_MeshVerticesNV[2].gl_Position = vec4(-1.0,  1.0, 0.0, 1.0); // Bottom Left
    gl_MeshVerticesNV[3].gl_Position = vec4( 1.0,  1.0, 0.0, 1.0); // Bottom Right

	// First Triangle
    gl_PrimitiveIndicesNV[0] = 0;
    gl_PrimitiveIndicesNV[1] = 1;
    gl_PrimitiveIndicesNV[2] = 2;
	gl_PrimitiveCountNV++;

	// Second Triangle
    gl_PrimitiveIndicesNV[3] = 2;
    gl_PrimitiveIndicesNV[4] = 1;
    gl_PrimitiveIndicesNV[5] = 3;
    gl_PrimitiveCountNV++;

	// Vertice Colors
	v_out[0].color = vec4(colors[0], 1.0);
  	v_out[1].color = vec4(colors[1], 1.0);
  	v_out[2].color = vec4(colors[2], 1.0);
  	v_out[3].color = vec4(colors[3], 1.0);
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