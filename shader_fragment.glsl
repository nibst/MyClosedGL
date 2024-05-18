#version 450 core

in vec4 color_vertex;
in vec4 world_position;
in vec4 model_position;
in vec4 frag_normal;

uniform int fragment_shader_type;

#define NO_SHADER 0
#define PHONG     3


uniform mat4 model;
uniform mat4 view;
uniform mat4 projecticon;

out vec4 color;


uniform vec3 diffuse_color;
uniform float diffuse_intensity;
uniform vec3 ambient_color;
uniform float ambient_intensity;
uniform vec3 specular_color;
uniform float specular_intensity;
uniform float shine_coefficient;


subroutine vec4 fragmentShader();



subroutine (fragmentShader) vec4 phongShader() {
    vec4 cameraPosition=inverse(view)[3];
    
    // Light is assumed to be at the camera
    vec4 l=normalize(cameraPosition-(model_position));
    vec4 n=normalize(inverse(transpose(model))*frag_normal);
    
    // Calculate lambert's diffuse term as Kd*I*dot(normal, light)
    vec3 diffuseTerm = diffuse_color * diffuse_intensity * max(0.0,dot(n,l));
    vec3 ambientTerm = ambient_color * vec3(ambient_intensity);

    // Calculate blinn-phong specular term
    vec4 h = normalize(2 * l);
    vec4 specularTerm = vec4(specular_color, 1.0) * vec4(vec3(specular_intensity),1.0) * pow(max(0, dot(n, h)), shine_coefficient); 

    // Final color
    return vec4(diffuseTerm + ambientTerm, 0.0) + specularTerm;
}
subroutine (fragmentShader) vec4 noFragmentShader() {
    return color_vertex;
}

void main()
{
    switch(fragment_shader_type) {
      case NO_SHADER: {
        color = color_vertex;//noFragmentShader();
        break;
      }
      case PHONG: {
        color = phongShader();
        break;
      }
      default: {
        color = noFragmentShader();
        break;
      }
    }
    //color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    
}
