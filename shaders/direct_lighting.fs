#version 460 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    //vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);  
    vec3 specular = vec3(0.0);

    if (gl_FrontFacing) {
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        specular = light.specular * (spec * material.specular);
    }
    else {
        norm = -norm;           // for back-face invert normal
    }

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
      
    vec3 result =clamp(ambient + diffuse + specular, 0.0, 1.0);
    FragColor = vec4(result, 1.0);
} 