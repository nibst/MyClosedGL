#version 450 core

layout( location = 0 ) in vec4 model_coefficients;
layout( location = 1 ) in vec4 normal_coefficients;
layout( location = 2 ) in vec4 texture_coefficients;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform vec3 specular_color;
uniform float specular_intensity;
uniform float shine_coefficient;

uniform bool close2GL_active;
uniform int vertex_shader_type;

#define NO_SHADER   0
#define GOURAUD_AD  1
#define GOURAUD_ADS 2

out vec4 color_vertex;
out vec4 world_position;
out vec4 model_position;
out vec4 frag_normal;

subroutine vec4 vertexShader();

// Lighting options
subroutine (vertexShader) vec4 noVertexShader() {
    return vec4(diffuse_color,1.) * diffuse_intensity;
}

subroutine (vertexShader) vec4 gouraudADshader() {
    vec4 light_source = vec4(2.0,-5.0,5.0,1.0);
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;    
    // Light is assumed to be at the camera
    vec4 l=normalize(light_source - world_position);
    vec4 n=normalize(frag_normal);
    // Calculate lambert's diffuse term as Kd*I*dot(normal, light)
    vec3 diffuseTerm = diffuse_color * diffuse_intensity * max(0.0,dot(n,l));
    vec3 ambientTerm = ambient_color * vec3(ambient_intensity);

    // Final color
    return vec4(diffuseTerm + ambientTerm, 1.0);
}

subroutine (vertexShader) vec4 gouraudADSshader() {
    vec4 light_source = vec4(2.0,-5.0,5.0,1.0);
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;       
    vec4 v=normalize(camera_position - world_position);
    vec4 l=normalize(light_source - world_position);
    vec4 n=normalize(frag_normal);
    
    // Calculate lambert's diffuse term as Kd*I*dot(normal, light)
    vec3 diffuseTerm = diffuse_color * diffuse_intensity * max(0.0,dot(n,l));
    vec3 ambientTerm = ambient_color * vec3(ambient_intensity);
    //blinn-phong specular term
    vec4 h = normalize(l + v);
    vec4 specularTerm = vec4(specular_color, 1.0) * vec4(vec3(specular_intensity),1.0) * pow(max(0, dot(n, h)), shine_coefficient); 

    // Final color
    return vec4(diffuseTerm + ambientTerm, 0.0) + specularTerm;
}

subroutine vec4 modelCoords();

// Vertex position options
subroutine (modelCoords) vec4 openGLCoords() {

    world_position = model * model_coefficients;
    return projection * view * model * model_coefficients;
}
subroutine (modelCoords) vec4 close2GLCoords() {
    return model_coefficients;
}
void main()
{
    frag_normal = normalize(inverse(transpose(model)) * normal_coefficients);
    frag_normal.w = 0.0; 
	if (close2GL_active) {
		gl_Position = close2GLCoords();
        color_vertex = noVertexShader();
        //texCoords = texture_coefficients;
	} else {
		gl_Position = openGLCoords();
        switch(vertex_shader_type) {
          case NO_SHADER: 
            color_vertex = noVertexShader();
            break;
          case GOURAUD_AD:
            color_vertex = gouraudADshader();
            break;
          case GOURAUD_ADS:
            color_vertex = gouraudADSshader();
            break;
          default:
            color_vertex = noVertexShader();
            break;
        }
    }  
}
