#include "Utils.h"




float Utils::Gaussrand(float deviation, float clamp)
{
    static float V1, V2, S;
    static int phase = 0;
    float X;

    if(phase == 0) {
        do {
            float U1 = (float)rand() / RAND_MAX;
            float U2 = (float)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
            } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);

    phase = 1 - phase;
    return glm::clamp(X * deviation, -clamp, clamp);
}


//------------------------------------------------------------------------------
// Function     	  : EvalHermite
// Description	    : 
//------------------------------------------------------------------------------
/**
 * EvalHermite(float pA, float pB, float vA, float vB, float u)
 * @brief Evaluates Hermite basis functions for the specified coefficients.
 */ 
inline float Utils::EvalHermite(float pA, float pB, float vA, float vB, float u)
{
  float u2=(u*u), u3=u2*u;
  float B0 = 2*u3 - 3*u2 + 1;
  float B1 = -2*u3 + 3*u2;
  float B2 = u3 - 2*u2 + u;
  float B3 = u3 - u;
  return( B0*pA + B1*pB + B2*vA + B3*vB );
}

// NORMALIZED GAUSSIAN INTENSITY MAP (N must be a power of 2)

//------------------------------------------------------------------------------
// Function     	  : CreateGaussianMap
// Description	    : 
//------------------------------------------------------------------------------
/**
 * CreateGaussianMap(int N)
 * 
 * Creates a 2D gaussian image using a hermite surface.
 */ 
unsigned char* Utils::CreateGaussianMap(int N)
{
  float *M = new float[2*N*N];
  unsigned char *B = new unsigned char[N*N];
  float X,Y,Y2,Dist;
  float Incr = 2.0f/N;
  int i=0;  
  int j = 0;
  Y = -1.0f;
  for (int y=0; y<N; y++, Y+=Incr)
  {
    Y2=Y*Y;
    X = -1.0f;
    for (int x=0; x<N; x++, X+=Incr, i+=2, j+=1)
    {
      Dist = (float)sqrt(X*X+Y2);
      if (Dist>1) Dist=1;
      M[i+1] = M[i] = EvalHermite(0.4f,0,0,0,Dist);// * (1 - noise);
      B[j] = (unsigned char)(M[i] * 255);
    }
  }
  delete[] M;
  return B;
}              


//------------------------------------------------------------------------------
// Function     	  : SkyCloud::_CreateSplatTexture
// Description	    : 
//------------------------------------------------------------------------------
/**
 * @fn SkyCloud::_CreateSplatTexture(unsigned int iResolution)
 * @brief Creates the texture map used for cloud particles.
 */ 
GLuint Utils::CreateSplatTexture(unsigned int size)
{
    unsigned char *splatTexture = CreateGaussianMap(size);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, splatTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] splatTexture;

    return texture;
}


GLuint Utils::glMvpUniform, Utils::glTextureUniform;
GLuint Utils::glPositionAttrib, Utils::glColorAttrib, Utils::glTexCoordsAtrib;
GLuint Utils::glTexture0;
GLuint Utils::glVBO1;