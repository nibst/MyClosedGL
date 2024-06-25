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
uniform mat4 projection;

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
    vec4 light_source = vec4(2.0,-5.0,5.0,1.0);
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;       
    vec4 v=normalize(camera_position - world_position);
    vec4 l=normalize(light_source - world_position);
    vec4 n=normalize(frag_normal);
    
    // Calculate lambert's diffuse term as Kd*I*dot(normal, light)
    vec3 diffuseTerm = diffuse_color * diffuse_intensity * max(0.0,dot(n,l));
    vec3 ambientTerm = ambient_color * vec3(ambient_intensity);

    // Calculate blinn-phong specular term
    vec4 h = normalize(l + v);
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
