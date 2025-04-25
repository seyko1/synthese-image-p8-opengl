#version 330

out vec4 fragColor;
in vec2 tCoord;

uniform sampler2D tex;
uniform vec2 pas;

void main() {
     vec4 moyenne = vec4(0.0);

     // exemple matrice de convolution 7x7 : mini flou de couleur magenta (entre le bleu et le rouge)
     for (int i = -1; i < 2; ++i) {
          for (int j = -1; j < 2; ++j) {
               moyenne += texture(tex, vec2(
                    tCoord.x + float(j) * pas.x,
                    tCoord.y + float(i) * pas.y
               ));
          }
     }

     moyenne /= 9.0;

     fragColor = moyenne;
}