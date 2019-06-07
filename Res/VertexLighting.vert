/**
*@file VertexLighting.vert
*/
#version 410

layout(location=0) in vec3 vPosition;
layout(location=1) in vec4 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
layout(location=0) out vec4 outColor;
layout(location=1) out vec2 outTexCoord;

//����.
struct AmbientLight{

vec3 color;
};

//�w�������C�g.
struct DirectionLight{

vec3 direction;
vec3 color;
};

//�|�C���g�E���C�g.

struct PointLight{

vec3 position[8];
vec3 color[8];
};

uniform AmbientLight ambientLight;
uniform DirectionLight directionalLight;
uniform PointLight pointLight;
uniform mat4x4 matMVP;

/**
*���_�V�F�[�_.
*/

void main(){

//�w�������C�g�̖��邳���v�Z.
float cosTheta = clamp(dot(vNormal,-directionalLight.direction),0.0,1.0);
outColor.rgb = (vColor.rgb * directionalLight.color * cosTheta);
vec3 lightColor = directionalLight.color * cosTheta;

//�|�C���g�E���C�g�̖��邳���v�Z.
for(int i = 0; i < 8; ++i){
if(dot(pointLight.color[i],pointLight.color[i]) != 0){

//�t���O�����g���烉�C�g�֌������x�N�g�����v�Z.
vec3 lightVector = pointLight.position[i] - vPosition;
vec3 lightDir = normalize(lightVector);
//�ʂ̌X����閾�邳�̕ω��ʂ��v�Z.
float cosTheta = clamp(dot(vNormal,lightDir),0.0,1.0);
//�����ɂ�閾�邳�̕ω��ʂ��v�Z.
float intensity = 1.0 / (1.0 + dot(lightVector,lightVector));
//�ω��ʂ��|�����킹�Ė��邳�����߁A���v�ɉ��Z.
lightColor += pointLight.color[i] * cosTheta * intensity;

}
}
outColor.rgb = vColor.rgb * lightColor;

//�����̌v�Z.
outColor.rgb += ambientLight.color;
outColor.a = vColor.a;

outTexCoord = vTexCoord;
gl_Position = matMVP * vec4(vPosition,1.0);
}